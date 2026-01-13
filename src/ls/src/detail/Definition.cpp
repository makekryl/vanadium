#include "detail/Definition.h"

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/core/utils/SemanticUtils.h>

#include "LSProtocol.h"
#include "LanguageServerConv.h"

namespace vanadium::ls::detail {

const ast::Node* FindNode(const core::SourceFile* file, lsp::Position pos) {
  return ast::utils::GetNodeAt(file->ast, file->ast.lines.GetPosition(conv::FromLSPPosition(pos)));
}

std::optional<SymbolSearchResult> FindSymbol(const core::SourceFile* file, const ast::Node* n) {
  if (!file->module) [[unlikely]] {
    return std::nullopt;
  }

  if (n->nkind != ast::NodeKind::Ident) {
    return std::nullopt;
  }

  const auto* target_node = n;
  switch (n->parent->nkind) {
    case ast::NodeKind::SelectorExpr: {
      const auto* se = n->parent->As<ast::nodes::SelectorExpr>();
      if (n != se->x) {
        target_node = se;
      }
      break;
    }
    case ast::NodeKind::AssignmentExpr: {
      const auto* ae = n->parent->As<ast::nodes::AssignmentExpr>();
      if (n == ae->property &&
          (ae->parent->nkind == ast::NodeKind::CompositeLiteral || ae->parent->nkind == ast::NodeKind::ParenExpr)) {
        const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, target_node);
        const auto type = core::checker::ext::ResolveAssignmentTarget(file, scope, ae);
        if (!type) {
          return std::nullopt;
        }
        return SymbolSearchResult{
            .node = n,
            .scope = nullptr,
            .type = type,
        };
      }
      break;
    }
    case ast::NodeKind::Field: {
      const auto* f = n->parent->As<ast::nodes::Field>();
      if (f->parent->nkind == ast::NodeKind::SubTypeDecl) {
        break;
      }

      const auto* owner = f->parent;

      const auto* structsym = core::checker::ResolveTypeSpecSymbol(file, owner->As<ast::nodes::TypeSpec>());
      if (!structsym) {
        return std::nullopt;
      }

      return SymbolSearchResult{
          .node = n,
          .scope = nullptr,
          .type = {.sym = structsym->Members()->Lookup(file->Text(*f->name))},
      };
    }
    case ast::NodeKind::EnumTypeDecl: {
      const auto* etd = n->parent->As<ast::nodes::EnumTypeDecl>();
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
          .type = {.sym = sym->Members()->Lookup(file->Text(n))},
      };
    }
    default:
      break;
  }

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, target_node);

  const auto sym = core::checker::ResolveExprSymbol(file, scope, target_node->As<ast::nodes::Expr>());
  if (!sym) {
    return std::nullopt;
  }
  return SymbolSearchResult{
      .node = n,
      .scope = scope,
      .type = sym,
  };
}

std::optional<SymbolSearchResult> FindSymbol(const core::SourceFile* file, lsp::Position pos) {
  const auto* n = FindNode(file, pos);
  if (!n) [[unlikely]] {
    return std::nullopt;
  }
  return FindSymbol(file, n);
}

const ast::Node* GetReadableDefinition(const ast::Node* n) {
  switch (n->nkind) {
#define READABLE_DEFINITION_CASE(Type, prop) \
  case ast::NodeKind::Type:                  \
    return std::addressof(*(n->As<ast::nodes::Type>()->prop));
    READABLE_DEFINITION_CASE(FuncDecl, name)
    READABLE_DEFINITION_CASE(TemplateDecl, name)
    READABLE_DEFINITION_CASE(ComponentTypeDecl, name)
    READABLE_DEFINITION_CASE(StructTypeDecl, name)
    READABLE_DEFINITION_CASE(ClassTypeDecl, name)
    READABLE_DEFINITION_CASE(EnumTypeDecl, name)
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
