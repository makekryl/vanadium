#include "utils/ASTUtils.h"

#include "ASTNodes.h"

namespace vanadium::core::ast {
namespace utils {

const Range& GetActualNameRange(const Node* n) {
  switch (n->nkind) {
#define CASE_NODE_WITH_OPTIONAL_NAME(ConcreteNode) \
  case NodeKind::ConcreteNode: {                   \
    const auto* m = n->As<nodes::ConcreteNode>();  \
    if (!m->name) break;                           \
    return m->name->nrange;                        \
  }
    CASE_NODE_WITH_OPTIONAL_NAME(Declarator)
    CASE_NODE_WITH_OPTIONAL_NAME(FuncDecl)
    CASE_NODE_WITH_OPTIONAL_NAME(StructTypeDecl)
    CASE_NODE_WITH_OPTIONAL_NAME(EnumTypeDecl)
    CASE_NODE_WITH_OPTIONAL_NAME(ClassTypeDecl)
#undef CASE_NODE_WITH_OPTIONAL_NAME
    default:
      break;
  }
  return n->nrange;
}

}  // namespace utils
}  // namespace vanadium::core::ast
