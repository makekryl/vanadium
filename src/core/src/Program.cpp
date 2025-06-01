#include "Program.h"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <ranges>

#include "Arena.h"
#include "LruCache.h"
#include "Parser.h"
#include "Semantic.h"

namespace vanadium::core {

void Program::Commit(const lib::Consumer<const ProgramModifier&>& modify) {
  using Action = ProgramModifier::SourceFilePathAction;
  modify({
      .add = Action::create<Program, &Program::AddFile>(this),
      .update = Action::create<Program, &Program::UpdateFile>(this),
      .drop = Action::create<Program, &Program::DropFile>(this),
  });
  Crossbind();
}

void Program::AddFile(const std::string& path) {
  auto& sf = files_[path];

  const auto contents = fs_->ReadFile(path, [&](std::size_t size) {
    return sf.arena.AllocStringBuffer(size).data();
  });
  if (!contents) {
    // TODO
    std::abort();
  }
  sf.path = path;

  sf.ast = ast::Parse(sf.arena, *contents);

  AttachFile(sf);
}

void Program::UpdateFile(const std::string& path) {
  auto& sf = files_.at(path);

  DetachFile(sf);

  sf.arena.Reset();

  const auto contents = fs_->ReadFile(sf.path, [&](std::size_t size) {
    return sf.arena.AllocStringBuffer(size).data();
  });
  sf.ast = ast::Parse(sf.arena, *contents);

  AttachFile(sf);
}

void Program::DropFile(const std::string& path) {
  auto& sf = files_[path];

  DetachFile(sf);

  files_.erase(path);
}

void Program::AttachFile(SourceFile& sf) {
  Bind(sf);

  modules_[sf.module.name] = &sf.module;

  sf.dirty = true;
}

void Program::DetachFile(SourceFile& sf) {
  auto& module = sf.module;

  for (auto& [dependency, entries] : module.dependencies) {
    dependency->dependents.erase(&module);
    for (auto& entry : entries) {
      auto& vec = entry.injected_to->augmentation;
      vec.erase(std::ranges::remove(vec, entry.provider).begin(), vec.end());
    }
  }

  for (auto* dependent : module.dependents) {
    dependent->sf->dirty = true;
  }
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

    auto* imported_module = modules_[import];
    if (imported_module == nullptr) {
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
    auto* imported_module = modules_[import];
    if (imported_module == nullptr) {
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
  const auto is_sf_dirty = [](const SourceFile& sf) {
    return sf.dirty;
  };

  std::vector<bool> resolution_set;
  const auto is_group_resolved = [&resolution_set] {
    return std::ranges::all_of(resolution_set, [](bool b) {
      return b;
    });
  };

  const auto register_dependency = [&](ModuleDescriptor& module, ModuleDescriptor* imported_module,
                                       DependencyEntry&& dependency) {
    imported_module->dependents.insert(&module);
    module.dependencies[imported_module].emplace_back(dependency);
  };

  // TODO: optimize LruCache to use static buffer
  lib::LruCache<std::string_view, std::pair<const semantic::Symbol*, ModuleDescriptor*>, 128> resolution_cache;
  for (auto& sf : files_ | std::views::values | std::views::filter(is_sf_dirty)) {
    auto& module = sf.module;
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
          ext_group.scope->augmentation.push_back(augmentation_table);  // TODO: should be reset on provider detach
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
          register_dependency(module, imported_module,
                              DependencyEntry{
                                  .provider = &imported_scope->symbols,
                                  .injected_to = module.scope,
                              });

          if (via != nullptr) {
            via->dependents.insert(&module);
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
    sf.dirty = false;
  }
}

}  // namespace vanadium::core
