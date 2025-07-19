#include <cstdio>
#include <format>
#include <ranges>

#include "ASTNodes.h"
#include "BuiltinRules.h"
#include "Context.h"
#include "Rule.h"
#include "Semantic.h"

namespace vanadium::lint {
namespace rules {

namespace {
struct ScopeChecker {
  Rule* rule;
  Context& ctx;

  void CheckScope(const core::semantic::Scope* scope, std::unordered_set<std::string_view>* outer_mutable_vars) {
    std::unordered_set<std::string_view> mutable_vars;
    for (auto* child : scope->GetChildren()) {
      CheckScope(child, &mutable_vars);
    }

    const auto ack_mut = [&](const core::ast::Node* n) {
      mutable_vars.insert(ctx.GetFile().Text(n));
    };

    const core::ast::NodeInspector inspector = [&](const core::ast::Node* n) {
      switch (n->nkind) {
        case core::ast::NodeKind::AssignmentExpr: {
          const auto* ae = n->As<core::ast::nodes::AssignmentExpr>();
          if (ae->property->nkind == core::ast::NodeKind::Ident) {
            ack_mut(ae->property);
          } else if (ae->property->nkind == core::ast::NodeKind::SelectorExpr) {
            const auto* se = n->As<core::ast::nodes::SelectorExpr>();
            while (se->x->nkind == core::ast::NodeKind::SelectorExpr) {
              se = se->x->As<core::ast::nodes::SelectorExpr>();
            }
            if (se->x->nkind == core::ast::NodeKind::Ident) {
              ack_mut(se->x);
            }
          }
          return false;
        }
        case core::ast::NodeKind::CallExpr: {
          const auto* ce = n->As<core::ast::nodes::CallExpr>();

          const core::ast::nodes::FuncDecl* func{nullptr};
          for (const auto [idx, arg] : ce->args->list | std::ranges::views::enumerate) {
            if (arg->nkind != core::ast::NodeKind::Ident) {
              continue;
            }
            if (func == nullptr) {
              const auto* res = scope->Resolve(ctx.GetFile().Text(ce->fun));
              if (res == nullptr || !(res->Flags() & core::semantic::SymbolFlags::kFunction)) {
                break;
              }
              func = res->Declaration()->As<core::ast::nodes::FuncDecl>();
            }
            if (func->params->list.size() <= std::size_t(idx)) {
              break;
            }
            const auto* param = func->params->list[idx];
            if (!param->direction) {
              continue;
            }
            if (param->direction->kind == core::ast::TokenKind::IN) {
              ack_mut(arg);
            }
          }
          return true;
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
      if (!(sym.Flags() & core::semantic::SymbolFlags::kVariable)) {
        continue;
      }
      const auto& name = sym.GetName();
      if (!mutable_vars.contains(name)) {
        ctx.Report(rule, sym.Declaration()->nrange, std::format("'{}' can be made const", name));
      } else {
        mutable_vars.erase(name);
      }
    }

    if (outer_mutable_vars) {
      outer_mutable_vars->merge(mutable_vars);
    }
  }
};
}  // namespace

PreferConst::PreferConst() : Rule("prefer-const") {}

void PreferConst::Exit(Context& ctx) {
  ScopeChecker checker{.rule = this, .ctx = ctx};
  for (const auto& sym : ctx.GetFile().module->scope->symbols.Enumerate() | std::ranges::views::values) {
    if (sym.Flags() & core::semantic::SymbolFlags::kFunction) {
      checker.CheckScope(sym.OriginatedScope(), nullptr);
    }
  }
};

}  // namespace rules
}  // namespace vanadium::lint
