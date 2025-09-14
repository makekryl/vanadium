#include <cstdio>
#include <format>
#include <ranges>
#include <string_view>
#include <unordered_set>

#include "ASTNodes.h"
#include "BuiltinRules.h"
#include "Context.h"
#include "Program.h"
#include "Rule.h"

namespace vanadium::lint {
namespace rules {

namespace {
const core::ImportDescriptor& FindImport(const core::ModuleDescriptor& module, std::string_view import, bool transit) {
  const auto range = module.ImportsOf(import);
  const auto& it = std::ranges::find_if(range, [&transit](const core::ImportDescriptor& idesc) {
    return idesc.transit == transit;
  });
  if (it != range.end()) {
    return *it;
  }

  // TODO: push error
  throw "this should never happen";
}
}  // namespace

NoUnusedImports::NoUnusedImports() : Rule("no-unused-imports") {}

void NoUnusedImports::Exit(Context& ctx) {
  const auto report_import = [&](const core::ast::nodes::ImportDecl* decl, std::string&& message) {
    ctx.Report(this, decl->nrange, std::move(message), Autofix::Removal(ctx.GetFile(), decl->parent->nrange));
  };

  std::unordered_set<std::string_view> unused_imports;
  std::unordered_set<std::string_view> unused_transit_imports;
  for (const auto& [import, desc] : ctx.GetFile().module->imports) {
    if (desc.is_public) {
      continue;
    }
    if (desc.transit) {
      unused_transit_imports.insert(import);
      continue;
    }
    unused_imports.insert(import);
  }

  for (const auto& dependency : ctx.GetFile().module->dependencies | std::ranges::views::keys) {
    unused_imports.erase(dependency->name);
  }
  for (const auto& import : unused_imports) {
    const auto* module = ctx.GetProgram().GetModule(import);
    if (module == nullptr || !module->sf->ast.errors.empty() ||
        ctx.GetFile().module->required_imports.contains(module->name)) {
      continue;
    }
    const auto* decl = FindImport(*ctx.GetFile().module, import, false).declaration;
    report_import(decl, std::format("imported module '{}' is not used directly", import));
  }

  for (const auto& used_transit : ctx.GetFile().module->transitive_dependency_providers) {
    unused_transit_imports.erase(used_transit->name);
  }
  for (const auto& import : unused_transit_imports) {
    const auto* module = ctx.GetProgram().GetModule(import);
    if (module == nullptr || !module->sf->ast.errors.empty()) {
      continue;
    }
    const auto* decl = FindImport(*ctx.GetFile().module, import, true).declaration;
    report_import(decl, std::format("public imports of imported module '{}' are not used", import));
  }
};

}  // namespace rules
}  // namespace vanadium::lint
