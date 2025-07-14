#include <cassert>

#include "ASTNodes.h"
#include "ASTTypes.h"

namespace vanadium::core::ast {

void Node::Accept(const NodeInspector& inspector) const {
#define FORWARD_ACCEPT(type)       \
  case NodeKind::type:             \
    As<type>()->Accept(inspector); \
    break;

  using namespace nodes;
  switch (nkind) {
    FORWARD_ACCEPT(Ident)
    FORWARD_ACCEPT(CompositeIdent)
    FORWARD_ACCEPT(NameIdent)
    FORWARD_ACCEPT(DeclStmt)
    FORWARD_ACCEPT(ExprStmt)
    case NodeKind::ErrorNode:
      break;
#include "gen/ASTInspector.inc"
    default:
      assert(false && "unhandled node kind in inspector code");
      break;
  }

#undef FORWARD_ACCEPT
}

}  // namespace vanadium::core::ast
