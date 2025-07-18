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
#include <print>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#include "ASTNodes.h"
#include "Arena.h"
#include "Bitset.h"
#include "LruCache.h"
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
  Bind(sf);

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
        auto& vec = entry.injected_to->augmentation;
        vec.erase(std::ranges::remove(vec, entry.provider).begin(), vec.end());

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

template <Program::ImportVisitorOptions Options>
bool Program::ForEachImport(const ModuleDescriptor& module, auto on_incomplete,
                            std::predicate<ModuleDescriptor*, ModuleDescriptor*> auto f, ModuleDescriptor* via) {
  bool incomplete = false;
  const auto report_incomplete = [&] {
    if (via != nullptr && !incomplete) {
      on_incomplete(via);
      incomplete = true;
    }
  };

  for (const auto& [import, descriptor] : module.imports) {
    if ((!Options.accept_private_imports && !descriptor.is_public) || descriptor.transit) {
      continue;
    }

    auto* imported_module = GetModule(import);
    if (!imported_module) {
      report_incomplete();
      continue;
    }

    if (!f(imported_module, via)) {
      return false;
    }
  }

  for (const auto& [import, descriptor] : module.imports) {
    if ((!Options.accept_private_imports && !descriptor.is_public) || !descriptor.transit) {
      continue;
    }
    auto* imported_module = GetModule(import);
    if (!imported_module) {
      report_incomplete();
      continue;
    }

    if (!ForEachImport<{.accept_private_imports = false}>(*imported_module, on_incomplete, f,
                                                          via ? via : imported_module)) {
      return false;
    }
  }

  return true;
}

namespace {
template <typename NodeTextProvider>
  requires std::is_invocable_r_v<std::string_view, NodeTextProvider, const ast::nodes::Ident*>
[[nodiscard]] std::optional<lib::Bitset> ResolveContribution(UnresolvedExternalsGroup& ext_group,
                                                             NodeTextProvider get_text,
                                                             const semantic::SymbolTable& table,
                                                             std::predicate<std::size_t> auto should_resolve_index) {
  std::optional<lib::Bitset> contribution;
  for (const auto& [idx, ident] : ext_group.idents | std::views::enumerate) {
    if (!should_resolve_index(idx)) {
      continue;
    }
    if (table.Lookup(get_text(ident))) {
      ext_group.resolution_set.Set(idx);
      if (!contribution) [[unlikely]] {
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

    // TODO: optimize LruCache to use static buffer
    lib::LruCache<std::string_view, std::pair<const semantic::Symbol*, ModuleDescriptor*>, 8> resolution_cache;
    auto& module = *sf.module;
    //
    module.unresolved.clear();
    resolution_cache.reset();

    const auto on_incomplete = [&](ModuleDescriptor* via) {
      // it may become useful after, we should keep an eye on it
      module.transitive_dependency_providers.insert(via);
    };

    for (auto& ext_group : module.externals) {
      if (!ext_group.augmentation_provider.empty() && !ext_group.augmentation_provider_injected) {
        const auto resolve_through = [&](const semantic::Symbol* sym, ModuleDescriptor* imported_module) {
          const semantic::SymbolTable* augmentation_table = sym->Members();
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

          ext_group.scope->augmentation.push_back(augmentation_table);

          register_dependency(module, imported_module,
                              DependencyEntry{
                                  .provider = augmentation_table,
                                  .injected_to = ext_group.scope,
                                  .ext_group = &ext_group,
                                  .contribution = *contribution_opt,
                                  .augmenting_locals = true,
                              });
        };

        if (const auto* cached_result = resolution_cache.get(ext_group.augmentation_provider)) {
          const auto& [sym, imported_module] = *cached_result;
          resolve_through(sym, imported_module);
        } else {
          ForEachImport<{.accept_private_imports = true}>(module, on_incomplete, [&](auto* imported_module, auto* via) {
            const auto* sym = imported_module->scope->ResolveDirect(ext_group.augmentation_provider);
            if (!sym) {
              // continue search
              return true;
            }

            resolve_through(sym, imported_module);
            if (via != nullptr) {
              register_transitive_dependency(module, via);
            }
            resolution_cache.put(ext_group.augmentation_provider, std::make_pair(sym, imported_module));
            return false;
          });
        }
      }

      if (!ext_group.IsResolved()) {
        ForEachImport<{.accept_private_imports = true}>(module, on_incomplete, [&](auto* imported_module, auto* via) {
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

          const bool is_new_dependency = register_dependency(module, imported_module,
                                                             DependencyEntry{
                                                                 .provider = &imported_table,
                                                                 .injected_to = module.scope,
                                                                 .ext_group = &ext_group,
                                                                 .contribution = *contribution_opt,
                                                                 .augmenting_locals = false,
                                                             });

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
    checker::PerformTypeCheck(*sf.module, sf.type_errors);

    sf.dirty = false;
  });

  for (auto* program : dependents_) {
    program->Crossbind();
  }
}

}  // namespace vanadium::core
