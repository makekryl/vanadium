#pragma once

#include <cstdio>
#include <format>
#include <ranges>

#include "ASTNodes.h"
#include "ASTTypes.h"
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
}  // namespace

class NoUnusedVars final : public Rule {
 public:
  NoUnusedVars() : Rule("no-unused-vars") {}

  void Register(const MatcherRegistrar&) const final {}
  void Check(Context&, const core::ast::Node*) final {}

  void Exit(Context& ctx) final {
    for (const auto& sym : ctx.GetFile().module->scope->symbols.Enumerate() | std::ranges::views::values) {
      if ((sym.Flags() & core::semantic::SymbolFlags::kFunction) ||
          (sym.Flags() & core::semantic::SymbolFlags::kTemplate)) {
        CheckScope(ctx, sym.OriginatedScope(), nullptr);
      }
    }
  };

 private:
  void CheckScope(Context& ctx, const core::semantic::Scope* scope,
                  std::unordered_set<std::string_view>* outer_used_vars) {
    std::unordered_set<std::string_view> used_vars;
    for (auto* child : scope->GetChildren()) {
      CheckScope(ctx, child, &used_vars);
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
            val->Accept(self);
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
        ctx.Report(this, GetVariableNameRange(sym.Declaration()),
                   std::format("{} '{}' is not used", is_argument ? "argument" : "variable", name));
      } else {
        used_vars.erase(name);
      }
    }

    if (outer_used_vars) {
      outer_used_vars->merge(used_vars);
    }
  }
};

}  // namespace rules
}  // namespace vanadium::lint
