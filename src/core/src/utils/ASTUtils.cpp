#include "utils/ASTUtils.h"

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "Scanner.h"

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

namespace {
template <IsNode ConcreteNode>
const ConcreteNode* BisectNodePos(const std::vector<ConcreteNode*>& list, pos_t pos) {
  auto it = std::lower_bound(list.begin(), list.end(), pos, [](ConcreteNode* n, pos_t p) {
    return n->nrange.begin <= p;
  });

  if (it == list.begin()) {
    return nullptr;
  }

  --it;
  if (!(*it)->nrange.Contains(pos)) {
    return nullptr;
  }

  return *it;
}
}  // namespace

const Node* GetNodeAt(const AST& ast, pos_t pos) {
  const Node* candidate;
  ast.root->Accept([&](this auto&& self, const Node* n) {
    bool pass;
    if (n->nkind == NodeKind::SelectorExpr) {
      const auto* head = TraverseSelectorExpressionStart(n->As<nodes::SelectorExpr>());
      pass = head->nrange.begin <= pos && pos <= n->nrange.end;
    } else {
      pass = n->nrange.Contains(pos);
    }

    if (pass) {
      candidate = n;

      switch (n->nkind) {
        case NodeKind::Module: {
          const auto* m = n->As<nodes::Module>();
          if (m->name) {
            Inspect(std::addressof(*m->name), self);
          }
          if (m->language) [[unlikely]] {
            Inspect(m->language, self);
          }
          if (m->with) [[unlikely]] {
            Inspect(m->with, self);
          }

          const auto* b = BisectNodePos(m->defs, pos);
          if (b) {
            Inspect(b, self);
          }
          return false;
        }

        case NodeKind::BlockStmt: {
          const auto* m = n->As<nodes::BlockStmt>();
          const auto* b = BisectNodePos(m->stmts, pos);
          if (b) {
            Inspect(b, self);
          }
          return false;
        }

        case NodeKind::CompositeLiteral: {
          const auto* m = n->As<nodes::CompositeLiteral>();
          const auto* b = BisectNodePos(m->list, pos);
          if (b) {
            Inspect(b, self);
          }
          return false;
        }

        case NodeKind::ParenExpr: {
          const auto* m = n->As<nodes::ParenExpr>();
          const auto* b = BisectNodePos(m->list, pos);
          if (b) {
            Inspect(b, self);
          }
          return false;
        }

        default:
          break;
      }

      return true;
    }
    return false;
  });
  return candidate;
}

std::optional<Range> ExtractAttachedComment(const AST& ast, const Node* n) {
  const auto anchor{n->nrange.begin};

  core::ast::Token preceding{};
  parser::Scanner scanner(ast.src);
  while (true) {
    const core::ast::Token tok = scanner.Scan();
    if (tok.range.end > anchor) {
      // EOF check can be avoided ig
      break;
    }
    if (preceding.kind == TokenKind::COMMENT && tok.kind == TokenKind::COMMENT) {
      continue;
    }
    preceding = tok;
  }

  if (preceding.kind != TokenKind::COMMENT) {
    return std::nullopt;
  }
  return Range{.begin = preceding.range.begin, .end = n->nrange.begin};
}

}  // namespace utils
}  // namespace vanadium::core::ast
