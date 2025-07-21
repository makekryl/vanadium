#include "detail/LanguageServerSymbolDef.h"

#include "AST.h"
#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LanguageServerConv.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls::detail {

namespace {
const core::semantic::Symbol* ResolvePropertyAssignmentTarget(const core::SourceFile* file,
                                                              const core::ast::nodes::AssignmentExpr* ae) {
  // const core::ast::Node* subject{nullptr};
  // for (const auto* n = ae->parent; n != nullptr; n = n->parent) {
  //   switch (n->nkind) {
  //     case core::ast::NodeKind::ValueDecl:
  //     case core::ast::NodeKind::FormalPar:
  //     case core::ast::NodeKind::CallExpr:
  //       subject = n;
  //     default:
  //       continue;
  //   }
  //   break;
  // }

  // if (!subject) {
  //   return nullptr;
  // }

  const auto property = file->Text(ae->property);
  const auto* scope = core::semantic::utils::FindScope(file->module->scope, ae);

  switch (ae->parent->nkind) {
    case core::ast::NodeKind::ParenExpr: {
      const auto* m = ae->parent->As<core::ast::nodes::ParenExpr>();

      const auto* ce = m->parent->As<core::ast::nodes::CallExpr>();
      const auto* callee_sym = core::checker::ResolveExprSymbol(file, scope, ce->fun);
      if (!(callee_sym->Flags() & core::semantic::SymbolFlags::kFunction)) {
        return nullptr;
      }

      return callee_sym->OriginatedScope()->ResolveDirect(property);
    }
    default:
      return nullptr;
  }
}
}  // namespace

const core::ast::Node* FindNode(const core::SourceFile* file, lsp::Position pos) {
  return core::ast::utils::GetNodeAt(file->ast, file->ast.lines.GetPosition(conv::FromLSPPosition(pos)));
}

std::optional<SymbolSearchResult> FindSymbol(const core::SourceFile* file, const core::ast::Node* n) {
  if (n->nkind != core::ast::NodeKind::Ident) {
    return std::nullopt;
  }

  const auto* target_node = n;
  switch (n->parent->nkind) {
    case core::ast::NodeKind::SelectorExpr: {
      const auto* se = n->parent->As<core::ast::nodes::SelectorExpr>();
      if (n != se->x) {
        target_node = se;
      }
      break;
    }
    case core::ast::NodeKind::AssignmentExpr: {
      const auto* ae = n->parent->As<core::ast::nodes::AssignmentExpr>();
      if (n == ae->property && (ae->parent->nkind == core::ast::NodeKind::CompositeLiteral ||
                                ae->parent->nkind == core::ast::NodeKind::ParenExpr)) {
        const auto* sym = ResolvePropertyAssignmentTarget(file, ae);
        if (!sym) {
          return std::nullopt;
        }
        return SymbolSearchResult{
            .node = n,
            .scope = nullptr,  // todo
            .symbol = sym,
        };
      }
      break;
    }
    case core::ast::NodeKind::Field: {
      const auto* f = n->parent->As<core::ast::nodes::Field>();
      if (f->parent->nkind == core::ast::NodeKind::StructTypeDecl) {
        const auto* stdecl = f->parent->As<core::ast::nodes::StructTypeDecl>();
        const auto* structsym = file->module->scope->ResolveDirect(file->Text(*stdecl->name));
        return SymbolSearchResult{
            .node = n,
            .scope = nullptr,
            .symbol = structsym->Members()->Lookup(file->Text(*f->name)),
        };
      }
      break;
    }
    default:
      break;
  }

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, target_node);
  const auto* sym = core::checker::ResolveExprSymbol(file, scope, target_node->As<core::ast::nodes::Expr>());
  if (!sym) {
    return std::nullopt;
  }

  return SymbolSearchResult{
      .node = n,
      .scope = scope,
      .symbol = sym,
  };
}

std::optional<SymbolSearchResult> FindSymbol(const core::SourceFile* file, lsp::Position pos) {
  return FindSymbol(file, FindNode(file, pos));
}

const core::ast::Node* GetReadableDeclaration(const core::ast::Node* n) {
  switch (n->nkind) {
    case core::ast::NodeKind::FuncDecl:
      return std::addressof(*(n->As<core::ast::nodes::FuncDecl>()->name));
    case core::ast::NodeKind::TemplateDecl:
      return std::addressof(*(n->As<core::ast::nodes::TemplateDecl>()->name));
    case core::ast::NodeKind::ComponentTypeDecl:
      return std::addressof(*(n->As<core::ast::nodes::ComponentTypeDecl>()->name));
    case core::ast::NodeKind::StructTypeDecl:
      return std::addressof(*(n->As<core::ast::nodes::StructTypeDecl>()->name));
    case core::ast::NodeKind::SubTypeDecl:
      return std::addressof(*(n->As<core::ast::nodes::SubTypeDecl>()->field->name));
    default:
      return n;
  }
}

}  // namespace vanadium::ls::detail
