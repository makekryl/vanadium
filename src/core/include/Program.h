#pragma once

#include <oneapi/tbb/spin_mutex.h>

#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "AST.h"
#include "ASTNodes.h"
#include "Arena.h"
#include "Bitset.h"
#include "FunctionRef.h"
#include "Semantic.h"
#include "TypeChecker.h"

namespace vanadium::core {

struct SourceFile;
struct ModuleDescriptor;

struct UnresolvedExternalsGroup {
  std::vector<const ast::nodes::Ident*> idents;
  std::vector<semantic::Scope*> scopes;
  std::string_view augmentation_provider;

  bool augmentation_provider_injected{false};
  lib::Bitset resolution_set;

  UnresolvedExternalsGroup(std::vector<const ast::nodes::Ident*>&& idents_, std::vector<semantic::Scope*>&& scopes_,
                           std::string_view augmentation_provider_ = {})
      : idents(std::move(idents_)),
        scopes(std::move(scopes_)),
        augmentation_provider(augmentation_provider_),
        resolution_set(idents_.size()) {}

  [[nodiscard]] bool IsResolved() const {
    return resolution_set.All();
  }
};

struct DependencyEntry {
  const semantic::SymbolTable* provider;
  semantic::Scope* injected_to;  // nullptr if injected to ext_group->scopes
  UnresolvedExternalsGroup* ext_group;
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

  std::vector<UnresolvedExternalsGroup> externals;
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

struct SourceFile {
  lib::Arena arena;
  std::string path;

  std::string src;
  ast::AST ast;

  std::vector<semantic::SemanticError> semantic_errors;
  std::vector<checker::TypeError> type_errors;

  std::optional<ModuleDescriptor> module;

  bool dirty;

  [[nodiscard]] std::string_view Text(const ast::Node* n) const noexcept {
    return ast.Text(n);
  }
  [[nodiscard]] std::string_view Text(const ast::Node& n) const noexcept {
    return ast.Text(n);
  }
  [[nodiscard]] std::string_view Text(const ast::Range& r) const noexcept {
    return ast.Text(r);
  }
};

class Program {
 public:
  using FileReadFn = lib::FunctionRef<void(std::string_view, std::string&)>;

  Program() = default;

 private:
  void UpdateFile(const std::string& path, const FileReadFn& read);
  void DropFile(const std::string& path);

  struct ProgramModifier {
    lib::Consumer<const std::string&, const FileReadFn&> update;
    lib::Consumer<const std::string&> drop;
  };

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

  void Update(const lib::Consumer<const ProgramModifier&>& modify);
  void Commit(const lib::Consumer<const ProgramModifier&>& modify);

  const ModuleDescriptor* GetModule(std::string_view name) const;
  ModuleDescriptor* GetModule(std::string_view name) {
    return const_cast<ModuleDescriptor*>(static_cast<const Program*>(this)->GetModule(name));
  }

  const SourceFile* GetFile(const std::string& path) const {
    const auto it = files_.find(path);
    return it == files_.end() ? nullptr : &it->second;
  }

  void AddReference(Program* program) {
    references_.push_back(program);
    program->dependents_.push_back(this);
  }

 private:
  void AttachFile(SourceFile&);
  void DetachFile(SourceFile&);

  struct ImportVisitorOptions {
    bool accept_private_imports;
  };
  template <ImportVisitorOptions>
  bool ForEachImport(const ModuleDescriptor& module, auto on_incomplete,
                     std::predicate<ModuleDescriptor*, ModuleDescriptor*> auto f, ModuleDescriptor* via = nullptr);
  void Crossbind();

  std::unordered_map<std::string, SourceFile> files_;
  tbb::speculative_spin_mutex files_mutex_;

  std::unordered_map<std::string_view, ModuleDescriptor*> modules_;
  tbb::speculative_spin_mutex modules_mutex_;

  std::vector<Program*> references_;
  std::vector<Program*> dependents_;
};

void Bind(SourceFile&);

}  // namespace vanadium::core
