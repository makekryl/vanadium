#include "utils/SemanticUtils.h"

#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/utils/ASTUtils.h>

#include <string_view>

namespace vanadium::core::semantic::utils {

std::string_view GetReadableTypeName(const SourceFile* sf, const semantic::Symbol* sym) {
  const auto lit = [&](const ast::Range& range) -> std::string_view {
    // kinda ASN.1 translation tax
    if (range.end >= range.begin) {
      return sf->Text(range);
    }
    return sf->Text(ast::Range{
        .begin = range.end,
        .end = range.begin,
    });
  };

  const auto* decl = sym->Declaration();
  switch (decl->nkind) {
    case ast::NodeKind::FuncDecl: {
      const auto* m = decl->As<ast::nodes::FuncDecl>();
      return lit(ast::Range{
          .begin = m->kind.range.begin,
          .end = m->name->nrange.end,
      });
    }
    case ast::NodeKind::TemplateDecl: {
      const auto* m = decl->As<ast::nodes::TemplateDecl>();
      return lit(ast::Range{
          .begin = m->type->nrange.begin,
          .end = m->name->nrange.end,
      });
    }
    case ast::NodeKind::ComponentTypeDecl: {
      const auto* m = decl->As<ast::nodes::ComponentTypeDecl>();
      constexpr std::uint32_t kTypeOffset = std::string_view{"type "}.length();
      return lit(ast::Range{
          .begin = m->nrange.begin + kTypeOffset,
          .end = m->name->nrange.end,
      });
    }
    case ast::NodeKind::StructTypeDecl: {
      const auto* m = decl->As<ast::nodes::StructTypeDecl>();
      return lit(ast::Range{
          .begin = m->kind.range.begin,
          .end = m->name->nrange.end,
      });
    }
    case ast::NodeKind::SubTypeDecl: {
      const auto* m = decl->As<ast::nodes::SubTypeDecl>();
      return lit(ast::Range{
          .begin = m->field->nrange.begin,
          .end = m->nrange.end,
      });
    }
    case ast::NodeKind::ClassTypeDecl: {
      const auto* m = decl->As<ast::nodes::ClassTypeDecl>();
      return lit(ast::Range{
          .begin = m->kind.range.begin,
          .end = m->name->nrange.end,
      });
    }
    case ast::NodeKind::EnumTypeDecl: {
      const auto* m = decl->As<ast::nodes::EnumTypeDecl>();
      return lit(ast::Range{
          .begin = m->nrange.begin,
          .end = m->name->nrange.end,
      });
    }
    case ast::NodeKind::ConstructorDecl: {
      return "class constructor";
    }
    default:
      return sf->Text(sym->Declaration());
  }
}
std::string_view GetReadableTypeName(const semantic::Symbol* sym) {
  if (sym->Flags() & semantic::SymbolFlags::kBuiltin) {
    return sym->GetName();
  }
  return GetReadableTypeName(ast::utils::SourceFileOf(sym->Declaration()), sym);
}

}  // namespace vanadium::core::semantic::utils
