#pragma once

#include <oneapi/tbb/spin_mutex.h>
#include <vanadium/asn1/ast/Asn1ModuleBasket.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/Bitset.h>
#include <vanadium/lib/DelimitedStringView.h>
#include <vanadium/lib/FunctionRef.h>

#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "vanadium/core/Semantic.h"
#include "vanadium/core/TypeChecker.h"

namespace vanadium::core {

struct SourceFile;
struct ModuleDescriptor;

struct ExternallyResolvedGroup {
  std::vector<const ast::nodes::Ident*> idents;
  std::vector<semantic::Scope*> scopes;

  lib::DelimitedStringView<','> augmentation_providers;
  semantic::SymbolFlags::Value augmentation_provider_required_flags;
  bool augmentation_providers_injected{false};

  lib::Bitset resolution_set;

  [[nodiscard]] bool IsResolved() const {
    return resolution_set.All();
  }
};

struct ModuleExternals {
  ExternallyResolvedGroup primary;                 // top-level definitions mandatory for dependent modules analysis
  ExternallyResolvedGroup secondary;               // other definitions that should be available from top-level scope
  std::vector<ExternallyResolvedGroup> augmented;  // scopes that needs to be augmented to complete resolution
};

struct DependencyEntry {
  const semantic::SymbolTable* provider;
  semantic::Scope* injected_to;  // nullptr if injected to ext_group->scopes
  ExternallyResolvedGroup* ext_group;
  lib::Bitset contribution;
  bool augmenting_locals;
};

struct ImportDescriptor {
  bool transit;  // TODO: (?) tagged pointer
  bool is_public;
  const ast::nodes::ImportDecl* declaration;
};

struct ModuleDescriptor {
  std::string_view name;
  SourceFile* sf;

  semantic::Scope* scope;

  std::unordered_multimap<std::string_view, ImportDescriptor> imports;

  std::unordered_map<ModuleDescriptor*, std::vector<DependencyEntry>> dependencies;
  std::unordered_set<ModuleDescriptor*> transitive_dependency_providers;
  std::unordered_set<ModuleDescriptor*> dependents;
  std::unordered_set<std::string_view> required_imports;

  ModuleExternals externals;
  std::vector<const ast::nodes::Ident*> unresolved;

  tbb::speculative_spin_mutex crossbind_mutex_;

  auto ImportsOf(std::string_view import) const {
    const auto [begin, end] = imports.equal_range(import);
    return std::ranges::subrange(begin, end) | std::views::values;
  }
  auto ImportsOf(std::string_view import) {
    auto range = static_cast<const ModuleDescriptor&>(*this).ImportsOf(import);
    return const_cast<std::remove_const_t<decltype(range)>&>(range);
  }
};

namespace AnalysisState {  // NOLINT(readability-identifier-naming)
enum Value : std::uint8_t {
  kDirty = 0,

  kBasicCrossbind = 1 << 0,
  kFullCrossbind = 1 << 1,
  kTypecheck = 1 << 2,

  kComplete = kBasicCrossbind | kFullCrossbind | kTypecheck
};
}

class Program;
struct SourceFile {
  lib::Arena arena;
  std::string path;

  Program* program;

  std::string src;
  ast::AST ast;

  std::vector<semantic::SemanticError> semantic_errors;
  std::vector<checker::TypeError> type_errors;

  std::optional<ModuleDescriptor> module;

  AnalysisState::Value analysis_state{AnalysisState::kDirty};
  bool skip_analysis{false};

  [[nodiscard]] std::string_view Text(const ast::Node* n) const noexcept {
    return ast.Text(n);
  }
  [[nodiscard]] std::string_view Text(const ast::Node& n) const noexcept {
    return ast.Text(n);
  }
  [[nodiscard]] std::string_view Text(const ast::Token* t) const noexcept {
    return ast.Text(t);
  }
  [[nodiscard]] std::string_view Text(const ast::Range& r) const noexcept {
    return ast.Text(r);
  }
};

class Program {
 public:
  using FileReadFn = lib::FunctionRef<void(const std::string& /* path */, std::string& /* buf */)>;

  Program() = default;

 private:
  void UpdateFile(const std::string& path, const FileReadFn& read);
  void DropFile(const std::string& path);

 public:
  const std::unordered_map<std::string, SourceFile>& Files() const {
    return files_;
  }

  auto Modules() const {
    return files_ | std::views::values | std::views::filter([](const SourceFile& sf) {
             return sf.module.has_value();
           }) |
           std::views::transform([](const SourceFile& sf) -> const ModuleDescriptor& {
             return *sf.module;
           });
  }

  struct ProgramModifier {
    lib::Consumer<const std::string& /* path */, const FileReadFn&> update;
    lib::Consumer<const std::string& /* path */> drop;
  };

  void Update(const lib::Consumer<const ProgramModifier&>& modify);
  void Commit(const lib::Consumer<const ProgramModifier&>& modify);

  const ModuleDescriptor* GetOwnModule(std::string_view name) const;
  ModuleDescriptor* GetOwnModule(std::string_view name) {
    return const_cast<ModuleDescriptor*>(static_cast<const Program*>(this)->GetOwnModule(name));
  }

  const ModuleDescriptor* GetModule(std::string_view name) const;
  ModuleDescriptor* GetModule(std::string_view name) {
    return const_cast<ModuleDescriptor*>(static_cast<const Program*>(this)->GetModule(name));
  }

  const SourceFile* GetFile(const std::string& path) const {
    const auto it = files_.find(path);
    return it == files_.end() ? nullptr : &it->second;
  }

  void AddReference(Program*);
  void SealReferences();
  auto References() const {
    return std::ranges::ref_view(references_);
  }

  bool VisitOwnModules(std::predicate<const ModuleDescriptor&> auto visit) const {
    for (const auto& module : Modules()) {
      if (!visit(module)) {
        return false;
      }
    }
    return true;
  }

  bool VisitAccessibleModules(std::predicate<const ModuleDescriptor&> auto visit) const {
    if (!VisitOwnModules(visit)) {
      return false;
    }
    for (const auto& ref : References()) {
      if (!ref->VisitOwnModules(visit)) {
        return false;
      }
    }
    return true;
  }

 private:
  void AttachFile(SourceFile&);
  void DetachFile(SourceFile&);

  void Crossbind(SourceFile&, ExternallyResolvedGroup&);
  void Analyze();

  std::unordered_map<std::string, SourceFile> files_;
  tbb::speculative_spin_mutex files_mutex_;

  std::unordered_map<std::string_view, ModuleDescriptor*> modules_;
  asn1::ast::Asn1ModuleBasket asn_modules_;

  std::unordered_set<Program*> explicit_references_;
  std::unordered_set<Program*> references_;
  std::unordered_set<Program*> dependents_;
};

}  // namespace vanadium::core
