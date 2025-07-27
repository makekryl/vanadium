#include "detail/LanguageServerSymbolDef.h"

#include "AST.h"
#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "detail/LanguageServerConv.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls::detail {

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
        const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, target_node);
        const auto* sym = core::checker::ext::ResolveAssignmentTarget(file, scope, ae);
        if (!sym) {
          return std::nullopt;
        }
        return SymbolSearchResult{
            .node = n,
            .scope = nullptr,
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
    case core::ast::NodeKind::CaseClause: {
      const auto* m = n->parent->As<core::ast::nodes::CaseClause>();
      const auto* ss = m->parent->As<core::ast::nodes::SelectStmt>();
      if (!ss->is_union) {
        break;
      }

      const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, target_node);
      const auto* usym = core::checker::ResolveExprType(file, scope, ss->tag);
      if (!usym || !(usym->Flags() & core::semantic::SymbolFlags::kUnion)) {
        return std::nullopt;
      }

      const auto property_name = file->Text(n);

      return SymbolSearchResult{
          .node = n,
          .scope = scope,
          .symbol = usym->Members()->Lookup(property_name),
      };
    }
    default:
      break;
  }

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, target_node);
  const auto* expected_type = core::checker::ext::DeduceExpectedType(file, scope, target_node);
  if (expected_type && (expected_type->Flags() & core::semantic::SymbolFlags::kEnum)) {
    return SymbolSearchResult{
        .node = n,
        .scope = nullptr,
        .symbol = expected_type->Members()->Lookup(file->Text(n)),
    };
  }

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

const core::ast::Node* GetReadableDefinition(const core::ast::Node* n) {
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
    case core::ast::NodeKind::Field:
      return std::addressof(*(n->As<core::ast::nodes::Field>()->name));
    case core::ast::NodeKind::Declarator:
      return std::addressof(*(n->As<core::ast::nodes::Declarator>()->name));
    case core::ast::NodeKind::CallExpr:
      return std::addressof(*(n->As<core::ast::nodes::CallExpr>()->fun));
    default:
      return n;
  }
}

}  // namespace vanadium::ls::detail
