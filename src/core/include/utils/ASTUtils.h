#pragma once

#include "ASTNodes.h"

namespace vanadium::core::ast {
namespace utils {

inline const Node* TraverseSelectorExpressionStart(const nodes::SelectorExpr* se) {
  while (se->x->nkind == core::ast::NodeKind::SelectorExpr) {
    se = se->x->As<core::ast::nodes::SelectorExpr>();
  }
  return se->x;
}

}  // namespace utils
}  // namespace vanadium::core::ast
