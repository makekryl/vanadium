#include <vanadium/ast/ASTNodes.h>

#include "BuiltinRules.h"
#include "Context.h"
#include "Rule.h"

namespace vanadium::lint {
namespace rules {

NoEmpty::NoEmpty() : Rule("no-empty") {}

void NoEmpty::Register(const MatcherRegistrar& match) const {
  match(ast::NodeKind::BlockStmt);
}

void NoEmpty::Check(Context& ctx, const ast::Node* node) {
  if (node->parent->nkind != ast::NodeKind::BlockStmt) {
    return;
  }

  const auto* b = node->As<ast::nodes::BlockStmt>();
  if (b->stmts.empty()) {
    ctx.Report(this, b->nrange, "empty block");
  }
}

}  // namespace rules
}  // namespace vanadium::lint
