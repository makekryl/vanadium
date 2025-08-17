#include "detail/References.h"

#include <memory>

#include "AST.h"
#include "ASTNodes.h"
#include "detail/Definition.h"
#include "utils/ASTUtils.h"

namespace vanadium::ls::detail {

void VisitLocalReferences(const core::SourceFile* file, lsp::Position pos, bool include_decl,
                          lib::Consumer<const core::ast::nodes::Ident*> accept) {
  const auto symres = detail::FindSymbol(file, pos);
  if (!symres || !symres->scope || symres->node->nkind != core::ast::NodeKind::Ident ||
      !(symres->symbol->Flags() & (core::semantic::SymbolFlags::kVariable | core::semantic::SymbolFlags::kArgument)) ||
      core::ast::utils::SourceFileOf(symres->node) != file) {
    return;
  }

  const auto name = file->Text(symres->node);

  const auto* decl = symres->symbol->Declaration();
  switch (decl->nkind) {
    case core::ast::NodeKind::Declarator: {
      decl = std::addressof(*decl->As<core::ast::nodes::Declarator>()->name);
      break;
    }
    case core::ast::NodeKind::FormalPar: {
      decl = std::addressof(*decl->As<core::ast::nodes::FormalPar>()->name);
      break;
    }
    default:
      break;
  }
  if (include_decl && decl->nkind == core::ast::NodeKind::Ident) {
    accept(decl->As<core::ast::nodes::Ident>());
  }

  const auto* scope = symres->scope;
  scope->Container()->Accept([&](this auto&& self, const core::ast::Node* n) {
    if (n->nkind == core::ast::NodeKind::AssignmentExpr) {
      const auto* ae = n->As<core::ast::nodes::AssignmentExpr>();
      if (ae->parent->nkind == core::ast::NodeKind::CompositeLiteral ||
          ae->parent->nkind == core::ast::NodeKind::ParenExpr) {
        core::ast::Inspect(ae->value, self);
        return false;
      }
      return true;
    }

    if (n->nkind == core::ast::NodeKind::SelectorExpr) {
      n = core::ast::utils::TraverseSelectorExpressionStart(n->As<core::ast::nodes::SelectorExpr>());
    }
    if (n->nkind != core::ast::NodeKind::Ident) {
      return true;
    }

    if (n != decl && file->Text(n) == name) {
      accept(n->As<core::ast::nodes::Ident>());
    }

    return true;
  });
}

}  // namespace vanadium::ls::detail
