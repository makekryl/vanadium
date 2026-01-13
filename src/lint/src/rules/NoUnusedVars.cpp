#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Semantic.h>

#include <format>
#include <ranges>

#include "BuiltinRules.h"
#include "Context.h"
#include "Rule.h"

namespace vanadium::lint {
namespace rules {

namespace {
ast::Range GetVariableNameRange(const ast::Node* decl) {
  switch (decl->nkind) {
    case ast::NodeKind::Declarator:
      return decl->As<ast::nodes::Declarator>()->name->nrange;
    case ast::NodeKind::FormalPar:
      return decl->As<ast::nodes::FormalPar>()->name->nrange;
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

  const auto inspector = [&](this auto&& self, const ast::Node* n) -> bool {
    switch (n->nkind) {
      case ast::NodeKind::FormalPars: {
        return false;
      }
      case ast::NodeKind::SelectorExpr: {
        const auto* se = n->As<ast::nodes::SelectorExpr>();
        ast::Inspect(ast::utils::TraverseSelectorExpressionStart(se), self);
        return false;
      }
      case ast::NodeKind::Declarator: {
        auto* val = n->As<ast::nodes::Declarator>()->value;
        if (val != nullptr) {
          ast::Inspect(val, self);
        }
        return false;
      }
      case ast::NodeKind::AssignmentExpr: {
        const auto* ae = n->As<ast::nodes::AssignmentExpr>();
        if (ae->parent->nkind == ast::NodeKind::CompositeLiteral || ae->parent->nkind == ast::NodeKind::ParenExpr) {
          ast::Inspect(ae->value, self);
          return false;
        }
        return true;
      }
      case ast::NodeKind::Ident: {
        used_vars.emplace(ctx.GetFile().Text(n));
        return false;
      }
      default: {
        return true;
      }
    }
  };

  const auto* container = scope->Container();
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
    const auto perform_check = [&] {
      CheckScope({this, ctx}, sym.OriginatedScope(), nullptr);
    };
    if (sym.Flags() & core::semantic::SymbolFlags::kFunction) {
      const auto* funcdecl = sym.Declaration()->As<ast::nodes::FuncDecl>();
      if (!funcdecl->external) [[likely]] {
        perform_check();
      }
    } else if (sym.Flags() & (core::semantic::SymbolFlags::kTemplate | core::semantic::SymbolFlags::kControl)) {
      perform_check();
    }
  }
};

}  // namespace rules
}  // namespace vanadium::lint
