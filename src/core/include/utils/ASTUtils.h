#pragma once

#include "AST.h"
#include "ASTNodes.h"

namespace vanadium::core::ast {
namespace utils {

inline SourceFile* SourceFileOf(const Node* n) {
  while (n->nkind != NodeKind::ConstructorDecl) {
    n = n->parent;
  }
  return n->As<RootNode>()->file;
}

inline const Node* GetNodeAt(pos_t pos, const AST& ast) {
  const Node* candidate;
  ast.root->Accept([&](const Node* n) {
    if (n->nrange.begin <= pos && n->nrange.end >= pos) {
      candidate = n;
      return true;
    }
    return false;
  });
  return candidate;
}

inline const Node* TraverseSelectorExpressionStart(const nodes::SelectorExpr* se) {
  while (se->x->nkind == core::ast::NodeKind::SelectorExpr) {
    se = se->x->As<core::ast::nodes::SelectorExpr>();
  }
  return se->x;
}

}  // namespace utils
}  // namespace vanadium::core::ast
