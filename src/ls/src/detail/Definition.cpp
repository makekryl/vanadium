#include "detail/Definition.h"

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
      const auto* owner = f->parent;

      // TODO: ResolveExprSymbol - it's no longer actually "Expr".
      const auto* structsym =
          core::checker::ResolveExprSymbol(file, file->module->scope, owner->As<core::ast::nodes::Expr>());
      if (!structsym) {
        return std::nullopt;
      }

      return SymbolSearchResult{
          .node = n,
          .scope = nullptr,
          .symbol = structsym->Members()->Lookup(file->Text(*f->name)),
      };
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
          .scope = nullptr,
          .symbol = usym->Members()->Lookup(property_name),
      };
    }
    case core::ast::NodeKind::EnumTypeDecl: {
      const auto* etd = n->parent->As<core::ast::nodes::EnumTypeDecl>();
      if (!etd->name) {
        return std::nullopt;
      }
      const auto* sym = file->module->scope->ResolveDirect(file->Text(*etd->name));
      if (!sym || !(sym->Flags() & core::semantic::SymbolFlags::kEnum)) {
        return std::nullopt;
      }
      return SymbolSearchResult{
          .node = n,
          .scope = nullptr,
          .symbol = sym->Members()->Lookup(file->Text(n)),
      };
    }
    default:
      break;
  }

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, target_node);

  const auto* sym = core::checker::ResolveExprSymbol(file, scope, target_node->As<core::ast::nodes::Expr>());

  const auto* expected_type = core::checker::ext::DeduceExpectedType(file, scope, target_node);
  if (expected_type && (expected_type->Flags() & core::semantic::SymbolFlags::kEnum)) {
    if (const auto* member_sym = expected_type->Members()->Lookup(file->Text(n)); member_sym) {
      return SymbolSearchResult{
          .node = n,
          .scope = nullptr,
          .symbol = member_sym,
      };
    }
  }

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
  const auto* n = FindNode(file, pos);
  if (!n) [[unlikely]] {
    return std::nullopt;
  }
  return FindSymbol(file, n);
}

const core::ast::Node* GetReadableDefinition(const core::ast::Node* n) {
  switch (n->nkind) {
#define READABLE_DEFINITION_CASE(Type, prop) \
  case core::ast::NodeKind::Type:            \
    return std::addressof(*(n->As<core::ast::nodes::Type>()->prop));
    READABLE_DEFINITION_CASE(FuncDecl, name)
    READABLE_DEFINITION_CASE(TemplateDecl, name)
    READABLE_DEFINITION_CASE(ComponentTypeDecl, name)
    READABLE_DEFINITION_CASE(StructTypeDecl, name)
    READABLE_DEFINITION_CASE(SubTypeDecl, field->name)
    READABLE_DEFINITION_CASE(Field, name)
    READABLE_DEFINITION_CASE(Declarator, name)
    READABLE_DEFINITION_CASE(CallExpr, fun)
#undef READABLE_DEFINITION_CASE
    default:
      return n;
  }
}

}  // namespace vanadium::ls::detail
