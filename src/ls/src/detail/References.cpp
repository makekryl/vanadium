#include "detail/References.h"

#include <memory>

#include "AST.h"
#include "ASTNodes.h"
#include "detail/Definition.h"
#include "utils/ASTUtils.h"

namespace vanadium::ls::detail {

void VisitLocalReferences(const core::SourceFile* file, lsp::Position pos, bool include_decl,
                          lib::Consumer<const ast::nodes::Ident*> accept) {
  const auto symres = detail::FindSymbol(file, pos);
  if (!symres || !symres->scope || symres->node->nkind != ast::NodeKind::Ident ||
      !(symres->type->Flags() & (core::semantic::SymbolFlags::kVariable | core::semantic::SymbolFlags::kArgument)) ||
      ast::utils::SourceFileOf(symres->node) != file) {
    return;
  }

  const auto name = file->Text(symres->node);

  const auto* decl = symres->type->Declaration();
  switch (decl->nkind) {
    case ast::NodeKind::Declarator: {
      decl = std::addressof(*decl->As<ast::nodes::Declarator>()->name);
      break;
    }
    case ast::NodeKind::FormalPar: {
      decl = std::addressof(*decl->As<ast::nodes::FormalPar>()->name);
      break;
    }
    default:
      break;
  }
  if (include_decl && decl->nkind == ast::NodeKind::Ident) {
    accept(decl->As<ast::nodes::Ident>());
  }

  const auto* scope = symres->scope;
  scope->Container()->Accept([&](this auto&& self, const ast::Node* n) {
    if (n->nkind == ast::NodeKind::AssignmentExpr) {
      const auto* ae = n->As<ast::nodes::AssignmentExpr>();
      if (ae->parent->nkind == ast::NodeKind::CompositeLiteral || ae->parent->nkind == ast::NodeKind::ParenExpr) {
        ast::Inspect(ae->value, self);
        return false;
      }
      return true;
    }

    if (n->nkind == ast::NodeKind::SelectorExpr) {
      n = ast::utils::TraverseSelectorExpressionStart(n->As<ast::nodes::SelectorExpr>());
    }
    if (n->nkind != ast::NodeKind::Ident) {
      return true;
    }

    if (n != decl && file->Text(n) == name) {
      accept(n->As<ast::nodes::Ident>());
    }

    return true;
  });
}

}  // namespace vanadium::ls::detail
