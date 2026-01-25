#include "vanadium/core/Program.h"

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

#include <oneapi/tbb/parallel_for_each.h>
#include <oneapi/tbb/spin_mutex.h>
#include <oneapi/tbb/task_group.h>

#include <vanadium/asn1/ast/Asn1ModuleBasket.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/Parser.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/Bitset.h>

#include "vanadium/core/Semantic.h"
#include "vanadium/core/TypeChecker.h"
#include "vanadium/core/utils/ImportVisitor.h"

namespace vanadium::core {

namespace {
inline AnalysisState::Value& operator|=(AnalysisState::Value& lhs, AnalysisState::Value rhs) {
  return lhs = static_cast<AnalysisState::Value>(lhs | rhs);
}

bool IsAsnModule(const SourceFile& sf) {
  return sf.path.ends_with(".asn");
}
}  // namespace

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
  Analyze();
}

void Program::UpdateFile(const std::string& path, const FileReadFn& read) {
  decltype(files_)::iterator it;
  bool inserted;
  {
    std::lock_guard lock(files_mutex_);
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
  if (!IsAsnModule(sf)) {
    sf.ast = ast::Parse(sf.arena, sf.src);
    sf.ast.root->file = &sf;

    AttachFile(sf);
  } else {
    // Attach is postponed until all other modules are parsed
    asn_modules_.Update(&sf, sf.src);
  }
}

void Program::DropFile(const std::string& path) {
  auto& sf = files_[path];

  DetachFile(sf);

  if (IsAsnModule(sf)) {
    // asnModules_.Drop((void*) &sf);
  }

  files_.erase(path);
}

void Program::AttachFile(SourceFile& sf) {
  semantic::Bind(sf);

  if (sf.module.has_value()) [[likely]] {
    sf.analysis_state = AnalysisState::kDirty;
  }

  {
    std::lock_guard lock(files_mutex_);
    modules_[sf.module->name] = std::addressof(*sf.module);
  }
}

void Program::DetachFile(SourceFile& sf) {
  if (!sf.module.has_value()) {
    return;
  }

  auto& module = *sf.module;

  for (auto& [dependency, entries] : module.dependencies) {
    std::lock_guard lock(dependency->crossbind_mutex_);

    dependency->dependents.erase(&module);
  }

  for (auto* dependent : module.dependents) {
    std::lock_guard lock(dependent->crossbind_mutex_);

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
          entry.ext_group->augmentation_providers_injected = false;
        }

        entry.contribution.Flip();  // it is no longer needed
        entry.ext_group->resolution_set &= entry.contribution;
      }
      dependent->dependencies.erase(it);
    }

    dependent->sf->analysis_state = AnalysisState::kDirty;
  }

  {
    std::lock_guard lock(files_mutex_);
    modules_.erase(module.name);
  }
}

void Program::AddReference(Program* program) {
  explicit_references_.emplace(program);
  program->direct_dependents_.emplace(this);
}

void Program::SealReferences() {
  const auto bind_ref = [&, pthis = this](this auto&& self, Program* referenced_program) -> void {
    pthis->references_.emplace(referenced_program);

    pthis->asn_modules_.AddReference(&referenced_program->asn_modules_);

    for (auto* tref : referenced_program->explicit_references_) {
      self(tref);
    }
  };
  for (auto* program : explicit_references_) {
    bind_ref(program);
  }
}

const ModuleDescriptor* Program::GetOwnModule(std::string_view name) const {
  const auto it = modules_.find(name);
  if (it != modules_.end()) {
    return it->second;
  }
  return nullptr;
}

const ModuleDescriptor* Program::GetModule(std::string_view name) const {
  if (const auto* mod = GetOwnModule(name); mod) {
    return mod;
  }

  for (const auto* ref : references_) {
    if (const auto* mod = ref->GetOwnModule(name); mod) {
      return mod;
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

void Program::Crossbind(SourceFile& sf, ExternallyResolvedGroup& ext_group) {
  auto& module = *sf.module;

  const auto register_dependency = [&](ModuleDescriptor& module, ModuleDescriptor* imported_module,
                                       DependencyEntry&& dependency) -> bool {
    const auto& [it, inserted] = module.dependencies.try_emplace(imported_module);
    it->second.emplace_back(dependency);
    {
      std::lock_guard lock(imported_module->crossbind_mutex_);
      imported_module->dependents.insert(&module);
    }
    return inserted;
  };
  const auto register_transitive_dependency = [&](ModuleDescriptor& module, ModuleDescriptor* transit_module) {
    module.transitive_dependency_providers.insert(transit_module);
    {
      std::lock_guard lock(transit_module->crossbind_mutex_);
      transit_module->dependents.insert(&module);
    }
  };

  const auto on_missing_module = [&](ModuleDescriptor* via, std::string_view /* missing_module */) {
    if (!via) {
      return;
    }
    // it may become useful after, we should keep an eye on it
    module.transitive_dependency_providers.insert(via);
  };

  if (!ext_group.augmentation_providers.sv().empty() && !ext_group.augmentation_providers_injected) {
    std::size_t total_providers{};
    std::size_t injected_providers{0};

    const auto inject_augmentation_provider = [&](const semantic::Symbol* sym, ModuleDescriptor* imported_module) {
      ++injected_providers;

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

      register_dependency(module, imported_module,
                          DependencyEntry{
                              .provider = augmentation_table,
                              .injected_to = nullptr,  // i.e. injected to ext_group->scopes
                              .ext_group = &ext_group,
                              .contribution = *contribution_opt,
                              .augmenting_locals = true,
                          });
    };

    const auto try_inject_augmentation_provider_via = [&](ModuleDescriptor* provider_module,
                                                          std::string_view provider_name) -> const semantic::Symbol* {
      if (const auto* sym = provider_module->scope->ResolveDirect(provider_name); sym) {
        inject_augmentation_provider(sym, provider_module);
        return sym;
      }
      return nullptr;
    };

    const auto search_for_augmentation_provider = [&, pthis = this](this auto&& self_search, ModuleDescriptor& pov,
                                                                    std::string_view provider_name) {
      if (ext_group.IsResolved()) {
        return;
      }
      ++total_providers;

      const auto check_module = [&](ModuleDescriptor* imported_module, auto* via) {
        const auto* sym = try_inject_augmentation_provider_via(imported_module, provider_name);
        if (!sym) {
          return false;
        }

        if (via != nullptr) {
          register_transitive_dependency(module, via);
        }

        const auto& extension_base = ast::utils::GetExtendsBase(sym->Declaration());
        for (const auto* ih : extension_base) {
          const auto ename = imported_module->sf->Text(ih);
          if (!try_inject_augmentation_provider_via(imported_module, ename)) {
            self_search(*imported_module, ename);
          }
        }
        return true;
      };

      semantic::VisitImports<{.accept_private_imports = true}>(pthis, pov, on_missing_module,
                                                               std::not_fn(check_module));
    };

    for (const auto& provider_name : ext_group.augmentation_providers.range()) {
      search_for_augmentation_provider(module, provider_name);
    }

    if (injected_providers == total_providers) {
      ext_group.augmentation_providers_injected = true;
    }
  }

  if (!ext_group.IsResolved()) {
    semantic::VisitImports<{.accept_private_imports = true}>(
        this, module, on_missing_module, [&](auto* imported_module, auto* via) {
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

          module.scope->augmentation.push_back(&imported_table);

          // better to double check than capture mutex in register_transitive_dependency
          if (is_new_dependency ||
              std::ranges::all_of(module.dependencies[imported_module], [](const DependencyEntry& entry) {
                return entry.augmenting_locals;
              })) {
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

// TODO: quick hack
extern thread_local bool do_reanalyse_program_deps;
thread_local bool do_reanalyse_program_deps = true;

void Program::Analyze() {
  tbb::parallel_for_each(asn_modules_.Keys<SourceFile>(), [&](SourceFile* sf) {
    if (sf->analysis_state != AnalysisState::kDirty) {
      // Hereby, incrementalized ASN.1 modules analysis resides here, in Program
      // Possible problems:
      //  1) if it is a multi-project solution, and Program::Analyze was called in a non-optimal order,
      //     i.e. project's B, which depends on A, crossbind will occur before project A's ASN modules transformation,
      //     B's crossbind will have invalid state as it did not know about those ASN modules existence
      // 2) Dirtyness propagation is powered by the crossbind's dependency detector
      return;
    }

    // todo: this is a copypaste from Program::UpdateFile
    DetachFile(*sf);
    sf->module = std::nullopt;
    sf->semantic_errors.clear();
    sf->arena.Reset();

    sf->ast = asn_modules_.Transform(sf, sf->arena);
    sf->ast.root->file = sf;
    AttachFile(*sf);
  });

  tbb::parallel_for_each(files_ | std::views::values, [&](SourceFile& sf) {
    auto& module = *sf.module;

    if (sf.analysis_state == AnalysisState::kDirty) {
      sf.type_errors.clear();
    }

    if (!(sf.analysis_state & AnalysisState::kBasicCrossbind)) {
      module.unresolved.clear();

      Crossbind(sf, module.externals.primary);

      sf.analysis_state |= AnalysisState::kBasicCrossbind;
    }

    if (!sf.skip_analysis && !(sf.analysis_state & AnalysisState::kFullCrossbind)) {
      Crossbind(sf, module.externals.secondary);
      for (auto& ext_group : module.externals.augmented) {
        Crossbind(sf, ext_group);
      }

      sf.analysis_state |= AnalysisState::kFullCrossbind;
    }
  });
  tbb::parallel_for_each(files_ | std::views::values, [&](SourceFile& sf) {
    if (!sf.skip_analysis && !(sf.analysis_state & AnalysisState::kTypecheck)) {
      checker::PerformTypeCheck(sf);

      sf.analysis_state |= AnalysisState::kTypecheck;
    }
  });

  if (do_reanalyse_program_deps) [[likely]] {
    for (auto* program : direct_dependents_) {
      program->Analyze();
    }
  }
}

}  // namespace vanadium::core
