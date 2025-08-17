#include "detail/Helpers.h"

#include "AST.h"
#include "ASTNodes.h"
#include "ASTTypes.h"
#include "Scanner.h"

namespace vanadium::ls::detail {

core::ast::pos_t FindModuleBodyStart(const core::ast::AST& ast, const core::ast::nodes::Module* module) {
  core::ast::parser::Scanner scanner(ast.src, module->name->nrange.end);
  while (true) {
    const core::ast::Token tok = scanner.Scan();
    switch (tok.kind) {
      case core::ast::TokenKind::LBRACE:
        return tok.range.end + 1;  // + '\n'
      case core::ast::TokenKind::kEOF:
        return -1;
      default:
        break;
    }
  }
}

core::ast::pos_t FindPositionAfterLastImport(const core::ast::AST& ast) {
  constexpr core::ast::pos_t kFailureRet = -1;

  if (ast.root->nodes.empty()) {
    return kFailureRet;
  }

  const auto* n = ast.root->nodes.front();
  if (n->nkind != core::ast::NodeKind::Module) {
    return kFailureRet;
  }

  const auto* module = n->As<core::ast::nodes::Module>();

  core::ast::pos_t last_import_range_end = FindModuleBodyStart(ast, module);
  for (const auto* def : module->defs) {
    if (def->def->nkind != core::ast::NodeKind::ImportDecl) {
      break;
    }
    last_import_range_end = def->nrange.end;
  }
  return last_import_range_end;
}

}  // namespace vanadium::ls::detail
