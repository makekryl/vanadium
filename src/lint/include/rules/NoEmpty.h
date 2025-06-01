#pragma once

#include "ASTNodes.h"
#include "Context.h"
#include "Rule.h"

namespace vanadium::lint {
namespace rules {

class NoEmpty final : public Rule {
 public:
  NoEmpty() : Rule("no-empty") {}

  void Register(const MatcherRegistrar& match) const final {
    match(core::ast::NodeKind::BlockStmt);
  }

  void Check(Context& ctx, const core::ast::Node* node) final {
    if (node->parent->nkind != core::ast::NodeKind::BlockStmt) {
      return;
    }

    const auto* b = node->As<core::ast::nodes::BlockStmt>();
    if (b->stmts.empty()) {
      ctx.Report(this, b->nrange, "empty block");
    }
  }
};

}  // namespace rules
}  // namespace vanadium::lint
