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
  Program() = default;

 private:
  void UpdateFile(const std::string& path, lib::FunctionRef<std::string_view(lib::Arena&)> read);
  void DropFile(const std::string& path);

  struct ProgramModifier {
    lib::Consumer<const std::string&, const std::function<std::string_view(lib::Arena&)>&> update;
    lib::Consumer<const std::string&> drop;
  };

 public:
  const std::unordered_map<std::string, SourceFile>& Files() const {
    return files_;
  }

  void Commit(const lib::Consumer<const ProgramModifier&>& modify);

  const ModuleDescriptor* GetModule(std::string_view name) const {
    const auto it = modules_.find(name);
    return it == modules_.end() ? nullptr : it->second;
  }

  const SourceFile* GetFile(const std::string& path) const {
    const auto it = files_.find(path);
    return it == files_.end() ? nullptr : &it->second;
  }

 private:
  void AttachFile(SourceFile&);
  void DetachFile(SourceFile&);

  template <bool>
  bool ForEachImport(const ModuleDescriptor& module, auto on_incomplete,
                     std::predicate<ModuleDescriptor*, ModuleDescriptor*> auto f,
                     ModuleDescriptor* via = nullptr);  // TODO invocable
  void Crossbind();

  std::unordered_map<std::string, SourceFile> files_;
  tbb::speculative_spin_mutex files_mutex_;

  std::unordered_map<std::string_view, ModuleDescriptor*> modules_;
  tbb::speculative_spin_mutex modules_mutex_;
};

void Bind(SourceFile&);

}  // namespace vanadium::core
