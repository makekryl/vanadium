#include "Program.h"

#include <oneapi/tbb/parallel_for_each.h>
#include <oneapi/tbb/spin_mutex.h>
#include <oneapi/tbb/task_group.h>

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#include "ASTNodes.h"
#include "Arena.h"
#include "Bitset.h"
#include "ImportVisitor.h"
#include "Parser.h"
#include "Semantic.h"
#include "TypeChecker.h"

namespace vanadium::core {

void Program::Update(const lib::Consumer<const ProgramModifier&>& modify) {
  tbb::task_group wg;
  modify({
      .update =
          [&](const std::string& path, const FileReadFn& read) {
            wg.run([this, path, read] {
              UpdateFile(path, read);
            });
          },
      .drop =
          [&](const std::string& path) {
            wg.run([this, path] {
              DropFile(path);
            });
          },
  });
  wg.wait();
}

void Program::Commit(const lib::Consumer<const ProgramModifier&>& modify) {
  Update(modify);
  Crossbind();
}

void Program::UpdateFile(const std::string& path, const FileReadFn& read) {
  decltype(files_)::iterator it;
  bool inserted;
  {
    tbb::speculative_spin_mutex::scoped_lock lock(files_mutex_);
    std::tie(it, inserted) = files_.try_emplace(std::move(path));
  }

  auto& sf = it->second;
  if (inserted) {
    sf.path = path;
    sf.program = this;
  } else {
    DetachFile(sf);
    sf.module = std::nullopt;
    sf.semantic_errors.clear();
    sf.arena.Reset();
  }

  read(path, sf.src);
  sf.ast = ast::Parse(sf.arena, sf.src);
  sf.ast.root->file = &sf;

  AttachFile(sf);
}

void Program::DropFile(const std::string& path) {
  auto& sf = files_[path];

  DetachFile(sf);

  files_.erase(path);
}

void Program::AttachFile(SourceFile& sf) {
  semantic::Bind(sf);

  if (sf.module.has_value()) [[likely]] {
    sf.dirty = true;
  }

  {
    tbb::speculative_spin_mutex::scoped_lock lock(files_mutex_);
    modules_[sf.module->name] = std::addressof(*sf.module);
  }
}

void Program::DetachFile(SourceFile& sf) {
  if (!sf.module.has_value()) {
    return;
  }

  auto& module = *sf.module;

  for (auto& [dependency, entries] : module.dependencies) {
    tbb::speculative_spin_mutex::scoped_lock lock(dependency->crossbind_mutex_);

    dependency->dependents.erase(&module);
  }

  for (auto* dependent : module.dependents) {
    tbb::speculative_spin_mutex::scoped_lock lock(dependent->crossbind_mutex_);

    auto it = dependent->dependencies.find(&module);
    if (it != dependent->dependencies.end()) {
      for (auto& entry : it->second) {
        const auto detach_from_scope = [&](semantic::Scope* scope) {
          auto& vec = scope->augmentation;
          vec.erase(std::ranges::remove(vec, entry.provider).begin(), vec.end());
        };
        if (entry.injected_to) {
          detach_from_scope(entry.injected_to);
        } else {
          for (auto* scope : entry.ext_group->scopes) {
            detach_from_scope(scope);
          }
        }

        if (entry.augmenting_locals) {
          entry.ext_group->augmentation_provider_injected = false;
        }

        entry.contribution.Flip();  // it is no longer needed
        entry.ext_group->resolution_set &= entry.contribution;
      }
      dependent->dependencies.erase(it);
    }

    dependent->sf->dirty = true;
  }

  {
    tbb::speculative_spin_mutex::scoped_lock lock(files_mutex_);
    modules_.erase(module.name);
  }
}

const ModuleDescriptor* Program::GetModule(std::string_view name) const {
  const auto it = modules_.find(name);
  if (it != modules_.end()) {
    return it->second;
  }

  for (const auto* ref : references_) {
    if (const auto* module = ref->GetModule(name); module) {
      return module;
    }
  }

  return nullptr;
}

namespace {
template <typename NodeTextProvider>
  requires std::is_invocable_r_v<std::string_view, NodeTextProvider, const ast::nodes::Ident*>
[[nodiscard]] std::optional<lib::Bitset> ResolveContribution(const ExternallyResolvedGroup& ext_group,
                                                             NodeTextProvider get_text,
                                                             const semantic::SymbolTable& table,
                                                             std::predicate<std::size_t> auto should_resolve_index) {
  std::optional<lib::Bitset> contribution;
  for (const auto& [idx, ident] : ext_group.idents | std::views::enumerate) {
    if (!should_resolve_index(idx)) {
      continue;
    }
    const auto name = get_text(ident);
    if (table.Lookup(name)) {
      if (!contribution.has_value()) [[unlikely]] {
        contribution.emplace(ext_group.resolution_set.Size());
      }
      contribution->Set(idx);
    }
  }
  return contribution;
}
}  // namespace

void Program::Crossbind() {
  const auto register_dependency = [&](ModuleDescriptor& module, ModuleDescriptor* imported_module,
                                       DependencyEntry&& dependency) -> bool {
    const auto& [it, inserted] = module.dependencies.try_emplace(imported_module);
    it->second.emplace_back(dependency);
    {
      tbb::speculative_spin_mutex::scoped_lock lock(imported_module->crossbind_mutex_);
      imported_module->dependents.insert(&module);
    }
    return inserted;
  };
  const auto register_transitive_dependency = [&](ModuleDescriptor& module, ModuleDescriptor* transit_module) {
    module.transitive_dependency_providers.insert(transit_module);
    {
      tbb::speculative_spin_mutex::scoped_lock lock(transit_module->crossbind_mutex_);
      transit_module->dependents.insert(&module);
    }
  };

  tbb::parallel_for_each(files_ | std::views::values, [&](SourceFile& sf) {
    if (!sf.dirty) {
      return;
    }

    auto& module = *sf.module;
    module.unresolved.clear();

    const auto on_incomplete = [&](ModuleDescriptor* via) {
      // it may become useful after, we should keep an eye on it
      module.transitive_dependency_providers.insert(via);
    };

    for (auto& ext_group : module.externals) {
      if (!ext_group.augmentation_provider.empty() && !ext_group.augmentation_provider_injected) {
        const auto resolve_through = [&](const semantic::Symbol* sym, ModuleDescriptor* imported_module) {
          const semantic::SymbolTable* augmentation_table =
              (sym->Flags() & semantic::SymbolFlags::kStructural) ? sym->Members() : &sym->OriginatedScope()->symbols;
          const auto contribution_opt = ResolveContribution(
              ext_group,
              [&sf](const ast::nodes::Ident* ident) {
                return sf.Text(ident);
              },
              *augmentation_table,
              [](const std::size_t) {
                return true;
              });

          if (!contribution_opt) {
            return;
          }
          ext_group.resolution_set |= *contribution_opt;

          for (auto* scope : ext_group.scopes) {
            scope->augmentation.push_back(augmentation_table);
          }
          ext_group.augmentation_provider_injected = true;

          register_dependency(module, imported_module,
                              DependencyEntry{
                                  .provider = augmentation_table,
                                  .injected_to = nullptr,  // i.e. injected to ext_group->scopes
                                  .ext_group = &ext_group,
                                  .contribution = *contribution_opt,
                                  .augmenting_locals = true,
                              });
        };

        semantic::VisitImports<{.accept_private_imports = true}>(
            this, module, on_incomplete, [&](auto* imported_module, auto* via) {
              const auto* sym = imported_module->scope->ResolveDirect(ext_group.augmentation_provider);
              if (!sym) {
                // continue search
                return true;
              }

              resolve_through(sym, imported_module);
              if (via != nullptr) {
                register_transitive_dependency(module, via);
              }
              return false;
            });
      }

      if (!ext_group.IsResolved()) {
        semantic::VisitImports<{.accept_private_imports = true}>(
            this, module, on_incomplete, [&](auto* imported_module, auto* via) {
              if (ext_group.IsResolved()) {
                return false;
              }

              const semantic::SymbolTable& imported_table = imported_module->scope->symbols;
              const auto contribution_opt = ResolveContribution(
                  ext_group,
                  [&sf](const ast::nodes::Ident* ident) {
                    return sf.Text(ident);
                  },
                  imported_table,
                  [&ext_group](const std::size_t idx) {
                    return !ext_group.resolution_set.Get(idx);
                  });

              if (!contribution_opt) {
                // there's still unresolved symbols as nothing new has been resolved
                // continue search
                return true;
              }
              ext_group.resolution_set |= *contribution_opt;

              // About ".injected_to = module.scope," - we've noticed that some unresolved symbols from this group
              // are actually are global symbols imported into this scope from another module.
              // We bind dependency to ext_group as we would want to recheck this group only if dependent symbol
              // goes out of the imported module scope.
              const bool is_new_dependency = register_dependency(module, imported_module,
                                                                 DependencyEntry{
                                                                     .provider = &imported_table,
                                                                     .injected_to = module.scope,
                                                                     .ext_group = &ext_group,
                                                                     .contribution = *contribution_opt,
                                                                     .augmenting_locals = false,
                                                                 });

              // better to double check than capture mutex in register_transitive_dependency
              if (is_new_dependency ||
                  std::ranges::all_of(module.dependencies[imported_module], [](const DependencyEntry& entry) {
                    return entry.augmenting_locals;
                  })) {
                module.scope->augmentation.push_back(&imported_table);
                if (via != nullptr) {
                  register_transitive_dependency(module, via);
                }
              }

              return !ext_group.IsResolved();
            });
      }

      if (!ext_group.IsResolved()) {  // IsResolved() is very fast, faster than checking individual bits
        for (std::size_t idx = 0; idx < ext_group.resolution_set.Size(); idx++) {
          if (!ext_group.resolution_set.Get(idx)) {
            module.unresolved.push_back(ext_group.idents[idx]);
          }
        }
      }
    }
  });

  tbb::parallel_for_each(files_ | std::views::values, [&](SourceFile& sf) {
    if (!sf.dirty) {
      return;
    }

    sf.type_errors.clear();
    checker::PerformTypeCheck(sf);

    sf.dirty = false;
  });

  for (auto* program : dependents_) {
    program->Crossbind();
  }
}

}  // namespace vanadium::core
