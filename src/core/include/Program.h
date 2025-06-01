#pragma once

#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "AST.h"
#include "ASTNodes.h"
#include "Arena.h"
#include "Filesystem.h"
#include "FunctionRef.h"
#include "Semantic.h"

namespace vanadium::core {

struct SourceFile;
struct ModuleDescriptor;

struct UnresolvedExternalsGroup {
  std::string_view augmentation_provider;
  std::vector<const ast::nodes::Ident*> idents;
  semantic::Scope* scope;
};

struct DependencyEntry {
  const semantic::SymbolTable* provider;
  semantic::Scope* injected_to;
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

  ast::AST ast;

  ModuleDescriptor module;
  std::vector<semantic::SemanticError> semantic_errors;

  bool dirty;

  [[nodiscard]] std::string_view Text(const ast::Node* n) const noexcept {
    return ast.Text(n);
  }
};

class Program {
 public:
  Program(const Filesystem* fs) : fs_(fs) {}

 private:
  struct ProgramModifier {
    using SourceFilePathAction = lib::Consumer<const std::string&>;
    SourceFilePathAction add;
    SourceFilePathAction update;
    SourceFilePathAction drop;
  };

  void AddFile(const std::string& path);
  void UpdateFile(const std::string& path);
  void DropFile(const std::string& path);

 public:
  const std::unordered_map<std::string, SourceFile>& Files() const {
    return files_;
  }

  void Commit(const lib::Consumer<const ProgramModifier&>& modify);

  const ModuleDescriptor* GetModule(std::string_view name) const {
    const auto it = modules_.find(name);
    return it == modules_.end() ? nullptr : it->second;
  }

  const Filesystem& GetFS() const {
    return *fs_;
  }

 private:
  void AttachFile(SourceFile&);
  void DetachFile(SourceFile&);

  template <bool>
  bool ForEachImport(const ModuleDescriptor& module, auto on_incomplete,
                     std::predicate<ModuleDescriptor*, ModuleDescriptor*> auto f,
                     ModuleDescriptor* via = nullptr);  // TODO invocable
  void Crossbind();

  const Filesystem* fs_;
  std::unordered_map<std::string, SourceFile> files_;
  std::unordered_map<std::string_view, ModuleDescriptor*> modules_;
};

void Bind(SourceFile&);

}  // namespace vanadium::core
