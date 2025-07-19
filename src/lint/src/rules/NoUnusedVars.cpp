#include <format>
#include <ranges>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "BuiltinRules.h"
#include "Context.h"
#include "Rule.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"

namespace vanadium::lint {
namespace rules {

namespace {
core::ast::Range GetVariableNameRange(const core::ast::Node* decl) {
  switch (decl->nkind) {
    case core::ast::NodeKind::Declarator:
      return decl->As<core::ast::nodes::Declarator>()->name->nrange;
    case core::ast::NodeKind::FormalPar:
      return decl->As<core::ast::nodes::FormalPar>()->name->nrange;
    default:
      return {};
  }
}

void CheckScope(const std::pair<NoUnusedVars*, Context&> rule_ctx, const core::semantic::Scope* scope,
                std::unordered_set<std::string_view>* outer_used_vars) {
  const auto& [rule, ctx] = rule_ctx;

  std::unordered_set<std::string_view> used_vars;
  for (auto* child : scope->GetChildren()) {
    CheckScope(rule_ctx, child, &used_vars);
  }

  const auto inspector = [&](this auto&& self, const core::ast::Node* n) -> bool {
    switch (n->nkind) {
      case core::ast::NodeKind::FormalPars: {
        return false;
      }
      case core::ast::NodeKind::SelectorExpr: {
        const auto* se = n->As<core::ast::nodes::SelectorExpr>();
        core::ast::Inspect(core::ast::utils::TraverseSelectorExpressionStart(se), self);
        return false;
      }
      case core::ast::NodeKind::Declarator: {
        auto* val = n->As<core::ast::nodes::Declarator>()->value;
        if (val != nullptr) {
          core::ast::Inspect(val, self);
        }
        return false;
      }
      case core::ast::NodeKind::Ident: {
        used_vars.emplace(ctx.GetFile().Text(n));
        return false;
      }
      default: {
        return true;
      }
    }
    return true;
  };

  const auto* container = scope->GetContainer();
  if (container != nullptr) {
    container->Accept(inspector);
  }

  for (const auto& sym : scope->symbols.Enumerate() | std::ranges::views::values) {
    const bool is_variable = (sym.Flags() & core::semantic::SymbolFlags::kVariable) != 0;
    const bool is_argument = (sym.Flags() & core::semantic::SymbolFlags::kArgument) != 0;
    if (!(is_variable || is_argument)) {
      continue;
    }
    const auto& name = sym.GetName();
    if (!used_vars.contains(name)) {
      ctx.Report(rule, GetVariableNameRange(sym.Declaration()),
                 std::format("{} '{}' is not used", is_argument ? "argument" : "variable", name));
    } else {
      used_vars.erase(name);
    }
  }

  if (outer_used_vars) {
    outer_used_vars->merge(used_vars);
  }
}
}  // namespace

NoUnusedVars::NoUnusedVars() : Rule("no-unused-vars") {}

void NoUnusedVars::Exit(Context& ctx) {
  for (const auto& sym : ctx.GetFile().module->scope->symbols.Enumerate() | std::ranges::views::values) {
    if ((sym.Flags() & (core::semantic::SymbolFlags::kFunction | core::semantic::SymbolFlags::kTemplate |
                        core::semantic::SymbolFlags::kControl))) {
      CheckScope({this, ctx}, sym.OriginatedScope(), nullptr);
    }
  }
};

}  // namespace rules
}  // namespace vanadium::lint
