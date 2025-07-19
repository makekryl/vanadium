#include "ASTNodes.h"
#include "BuiltinRules.h"
#include "Context.h"
#include "Rule.h"

namespace vanadium::lint {
namespace rules {

NoEmpty::NoEmpty() : Rule("no-empty") {}

void NoEmpty::Register(const MatcherRegistrar& match) const {
  match(core::ast::NodeKind::BlockStmt);
}

void NoEmpty::Check(Context& ctx, const core::ast::Node* node) {
  if (node->parent->nkind != core::ast::NodeKind::BlockStmt) {
    return;
  }

  const auto* b = node->As<core::ast::nodes::BlockStmt>();
  if (b->stmts.empty()) {
    ctx.Report(this, b->nrange, "empty block");
  }
}

}  // namespace rules
}  // namespace vanadium::lint
