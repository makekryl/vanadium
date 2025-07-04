#include "domain/LanguageServerSymbolDef.h"

namespace vanadium::ls::domain {

const core::ast::Node* GetReadableDeclaration(const core::semantic::Symbol* sym) {
  const auto* n = sym->Declaration();
  switch (n->nkind) {
    case core::ast::NodeKind::FuncDecl:
      return std::addressof(*(n->As<core::ast::nodes::FuncDecl>()->name));
    case core::ast::NodeKind::TemplateDecl:
      return std::addressof(*(n->As<core::ast::nodes::TemplateDecl>()->name));
    default:
      return n;
  }
}

}  // namespace vanadium::ls::domain
