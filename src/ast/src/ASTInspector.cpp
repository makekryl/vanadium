#include <cassert>

#include "vanadium/ast/AST.h"
#include "vanadium/ast/ASTNodes.h"
#include "vanadium/ast/ASTTypes.h"

namespace vanadium::ast {

void Node::Accept(const NodeInspector& inspector) const {
#define FORWARD_ACCEPT(type)       \
  case NodeKind::type:             \
    As<type>()->Accept(inspector); \
    break;

  using namespace nodes;
  switch (nkind) {
    FORWARD_ACCEPT(RootNode)
    FORWARD_ACCEPT(Ident)
    FORWARD_ACCEPT(CompositeIdent)
    FORWARD_ACCEPT(DeclStmt)
    FORWARD_ACCEPT(ExprStmt)
    case NodeKind::ErrorNode:
      break;
#include "vanadium/ast/gen/ASTInspector.inc"
    default:
      assert(false && "unhandled node kind in inspector code");
      break;
  }

#undef FORWARD_ACCEPT
}

}  // namespace vanadium::ast
