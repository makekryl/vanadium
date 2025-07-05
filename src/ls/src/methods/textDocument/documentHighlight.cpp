#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolAdjustments.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::DocumentHighlightResults> methods::textDocument::documentHighlight::operator()(
    LsContext& ctx, const lsp::DocumentHighlightParams& params) {
  const auto path = ctx->FileUriToPath(params.textDocument.uri);
  const auto* file = ctx->program.GetFile(path);

  const auto* n = core::ast::utils::GetNodeAt(file->ast, file->ast.lines.GetPosition(core::ast::Location{
                                                             .line = params.position.line,
                                                             .column = params.position.character,
                                                         }));

  if (n->nkind != core::ast::NodeKind::Ident) {
    return nullptr;
  }

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, n);

  const auto sym_name = n->On(file->ast.src);
  if (const auto* sym = scope->Resolve(sym_name); sym) {
    if (!(sym->Flags() & core::semantic::SymbolFlags::kVariable) &&
        !(sym->Flags() & core::semantic::SymbolFlags::kArgument)) {
      // TODO
      return nullptr;
    }

    std::vector<lsp::DocumentHighlight> refs;

    const core::ast::Node* container = sym->Declaration();
    while (container->nkind != core::ast::NodeKind::BlockStmt) {
      container = container->parent;
    }
    container->Accept([&](const core::ast::Node* vn) {
      if (vn->nkind == core::ast::NodeKind::Ident && file->ast.Text(vn) == sym_name) {
        const bool is_write = (vn->parent->nkind == core::ast::NodeKind::AssignmentExpr) &&
                              (vn->parent->As<core::ast::nodes::AssignmentExpr>()->property == vn);
        refs.emplace_back(lsp::DocumentHighlight{
            .range = conv::ToLSPRange(vn->nrange, file->ast),
            .kind = is_write ? lsp::DocumentHighlightKind::kWrite : lsp::DocumentHighlightKind::kRead,
        });
      }
      return true;
    });

    return refs;
  }

  return nullptr;
}
}  // namespace vanadium::ls
