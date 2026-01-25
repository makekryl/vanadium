#include "vanadium/ls/detail/Helpers.h"

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/Scanner.h>

namespace vanadium::ls::detail {

ast::pos_t FindModuleBodyStart(const ast::AST& ast, const ast::nodes::Module* module) {
  ast::parser::Scanner scanner(ast.src, module->name->nrange.end);
  while (true) {
    const ast::Token tok = scanner.Scan();
    switch (tok.kind) {
      case ast::TokenKind::LBRACE:
        return tok.range.end + 1;  // + '\n'
      case ast::TokenKind::kEOF:
        return -1;
      default:
        break;
    }
  }
}

ast::pos_t FindPositionAfterLastImport(const ast::AST& ast) {
  constexpr ast::pos_t kFailureRet = -1;

  if (ast.root->nodes.empty()) {
    return kFailureRet;
  }

  const auto* n = ast.root->nodes.front();
  if (n->nkind != ast::NodeKind::Module) {
    return kFailureRet;
  }

  const auto* module = n->As<ast::nodes::Module>();

  ast::pos_t last_import_range_end = FindModuleBodyStart(ast, module);
  for (const auto* def : module->defs) {
    if (def->def->nkind != ast::NodeKind::ImportDecl) {
      break;
    }
    last_import_range_end = def->nrange.end;
  }
  return last_import_range_end;
}

}  // namespace vanadium::ls::detail
