#include "vanadium/lint/rules/NoUnnecessaryValueof.h"

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/core/utils/SemanticUtils.h>

#include "vanadium/lint/Context.h"
#include "vanadium/lint/Rule.h"

namespace vanadium::lint {
namespace rules {

NoUnnecessaryValueof::NoUnnecessaryValueof() : Rule("no-unnecessary-valueof") {}

void NoUnnecessaryValueof::Register(const MatcherRegistrar& match) const {
  match(ast::NodeKind::CallExpr);
}

void NoUnnecessaryValueof::Check(Context& ctx, const ast::Node* node) {
  const auto* m = node->As<ast::nodes::CallExpr>();
  if (m->fun->nkind != ast::NodeKind::Ident || m->args->list.size() != 1) {
    return;
  }

  const auto callee = ctx.GetFile().Text(m->fun);
  if (callee != "valueof") {
    return;
  }

  const auto* scope = core::semantic::utils::FindScope(ctx.GetFile().module->scope, m);
  auto argtype = core::checker::ResolveExprSymbol(&ctx.GetFile(), scope, m->args->list[0]);

  if (!argtype || (argtype->Flags() & core::semantic::SymbolFlags::kTemplate)) {
    return;
  }

  if (argtype->Flags() & core::semantic::SymbolFlags::kFunction) {
    argtype = core::checker::ResolveCallableReturnType(ast::utils::SourceFileOf(argtype->Declaration()),
                                                       argtype->Declaration()->As<ast::nodes::Decl>());
    if (!argtype) {
      return;
    }
  }

  if (argtype.restriction == core::checker::TemplateRestrictionKind::kNone) [[unlikely]] {
    ctx.Report(this, m->fun->nrange, "valueof's argument is already a value");
  }
}

}  // namespace rules
}  // namespace vanadium::lint
