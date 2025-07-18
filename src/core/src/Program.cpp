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
#include <unordered_map>

#include "Arena.h"
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

    for (const auto& entry : dependent->dependencies[&module]) {
      auto& vec = entry.injected_to->augmentation;
      vec.erase(std::ranges::remove(vec, entry.provider).begin(), vec.end());
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

template <bool AcceptPrivateImports>
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
    if ((!AcceptPrivateImports && !descriptor.is_public) || descriptor.transit) {
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
    if ((!AcceptPrivateImports && !descriptor.is_public) || !descriptor.transit) {
      continue;
    }
    auto* imported_module = GetModule(import);
    if (!imported_module) {
      report_incomplete();
      continue;
    }

    if (!ForEachImport<false>(*imported_module, on_incomplete, f, via ? via : imported_module)) {
      return false;
    }
  }

  return true;
}

void Program::Crossbind() {
  const auto register_dependency = [&](ModuleDescriptor& module, ModuleDescriptor* imported_module,
                                       DependencyEntry&& dependency) {
    imported_module->dependents.insert(&module);
    module.dependencies[imported_module].emplace_back(dependency);
  };

  tbb::parallel_for_each(files_ | std::views::values, [&](SourceFile& sf) {
    if (!sf.dirty) {
      return;
    }

    std::vector<bool> resolution_set;
    const auto is_group_resolved = [&resolution_set] {
      return std::ranges::all_of(resolution_set, [](bool b) {
        return b;
      });
    };

    // TODO: optimize LruCache to use static buffer
    lib::LruCache<std::string_view, std::pair<const semantic::Symbol*, ModuleDescriptor*>, 128> resolution_cache;
    auto& module = *sf.module;
    //
    module.unresolved.clear();
    resolution_cache.reset();

    const auto on_incomplete = [&](ModuleDescriptor* via) {
      module.transitive_dependency_providers.insert(via);
    };

    for (const auto& ext_group : module.externals) {
      resolution_set.clear();
      resolution_set.resize(ext_group.idents.size(), false);

      if (!ext_group.augmentation_provider.empty()) {
        const semantic::SymbolTable* augmentation_table{nullptr};

        const auto resolve_via = [&](const semantic::Symbol* sym, ModuleDescriptor* imported_module) {
          augmentation_table = sym->Members();
          ext_group.scope->augmentation.push_back(augmentation_table);

          tbb::speculative_spin_mutex::scoped_lock lock(imported_module->crossbind_mutex_);
          register_dependency(module, imported_module,
                              DependencyEntry{
                                  .provider = augmentation_table,
                                  .injected_to = ext_group.scope,
                              });
        };

        if (const auto* cached_result = resolution_cache.get(ext_group.augmentation_provider)) {
          const auto& [sym, imported_module] = *cached_result;
          resolve_via(sym, imported_module);
        } else {
          ForEachImport<true>(module, on_incomplete, [&](auto* imported_module, auto* via) {
            if (const auto* sym = imported_module->scope->ResolveDirect(ext_group.augmentation_provider)) {
              resolve_via(sym, imported_module);
              if (via != nullptr) {
                via->dependents.insert(&module);
                module.transitive_dependency_providers.insert(via);
              }
              resolution_cache.put(ext_group.augmentation_provider, std::make_pair(sym, imported_module));
              return false;
            }
            return true;
          });
        }

        if (augmentation_table) {
          for (const auto& [idx, ident] : ext_group.idents | std::views::enumerate) {
            if (augmentation_table->Lookup(sf.Text(ident))) {
              resolution_set[idx] = true;
            }
          }
        }
      }

      ForEachImport<true>(module, on_incomplete, [&](auto* imported_module, auto* via) {
        if (is_group_resolved()) {
          return false;
        }

        const auto* imported_scope = imported_module->scope;
        bool depends{false};
        for (const auto& [idx, ident] : ext_group.idents | std::views::enumerate) {
          if (resolution_set[idx]) {
            continue;
          }
          if (imported_scope->ResolveDirect(sf.Text(ident))) {
            resolution_set[idx] = true;
            depends = true;
          }
        }
        if (depends) {
          module.scope->augmentation.push_back(&imported_scope->symbols);
          {
            tbb::speculative_spin_mutex::scoped_lock lock(imported_module->crossbind_mutex_);
            register_dependency(module, imported_module,
                                DependencyEntry{
                                    .provider = &imported_scope->symbols,
                                    .injected_to = module.scope,
                                });
          }

          if (via != nullptr) {
            {
              tbb::speculative_spin_mutex::scoped_lock lock(via->crossbind_mutex_);
              via->dependents.insert(&module);
            }
            module.transitive_dependency_providers.insert(via);
          }
        }

        return !is_group_resolved();
      });

      for (std::size_t idx = 0; idx < resolution_set.size(); idx++) {
        if (!resolution_set[idx]) {
          module.unresolved.push_back(ext_group.idents[idx]);
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
