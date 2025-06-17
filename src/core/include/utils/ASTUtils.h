#pragma once

#include "AST.h"
#include "ASTNodes.h"

namespace vanadium::core::ast {
namespace utils {

inline const Node* TraverseSelectorExpressionStart(const nodes::SelectorExpr* se) {
  while (se->x->nkind == core::ast::NodeKind::SelectorExpr) {
    se = se->x->As<core::ast::nodes::SelectorExpr>();
  }
  return se->x;
}

inline SourceFile* SourceFileOf(const Node* n) {
  while (n->nkind != NodeKind::Root) {
    n = n->parent;
  }
  return n->As<RootNode>()->file;
}

// TODO: utilize binary search assuming node lists are sorted
inline const Node* GetNodeAt(const AST& ast, pos_t pos) {
  const Node* candidate;
  ast.root->Accept([&](const Node* n) {
    bool pass;
    if (n->nkind == NodeKind::SelectorExpr) {
      const auto* head = TraverseSelectorExpressionStart(n->As<nodes::SelectorExpr>());
      pass = head->nrange.begin <= pos && pos <= n->nrange.end;
    } else {
      pass = n->nrange.Contains(pos);
    }

    if (pass) {
      candidate = n;
      return true;
    }
    return false;
  });
  return candidate;
}

}  // namespace utils
}  // namespace vanadium::core::ast
