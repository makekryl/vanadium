#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "detail/Definition.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {

template <>
rpc::ExpectedResult<lsp::RenameResult> methods::textDocument::rename::operator()(LsContext& ctx,
                                                                                 const lsp::RenameParams& params) {
  // TODO: shorten the handler, use ctx->WithFile
  const auto& resolution = ctx->ResolveFileUri(params.textDocument.uri);
  if (!resolution) {
    return nullptr;
  }
  const auto& [project, path] = *resolution;
  const auto* file = project.program.GetFile(path);

  const auto* n = core::ast::utils::GetNodeAt(file->ast, file->ast.lines.GetPosition(core::ast::Location{
                                                             .line = params.position.line,
                                                             .column = params.position.character,
                                                         }));

  if (n->nkind != core::ast::NodeKind::Ident) {
    return nullptr;
  }

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, n);
  const auto sym_name = n->On(file->ast.src);
  const auto* sym = scope->Resolve(sym_name);
  if (!sym || !(sym->Flags() & (core::semantic::SymbolFlags::kVariable | core::semantic::SymbolFlags::kArgument))) {
    // TODO: support fields
    return nullptr;
  }

  lsp::WorkspaceEdit result;
  auto& changes = result.changes.emplace();
  auto& edits = changes[params.textDocument.uri];

  const core::ast::Node* container = sym->Declaration();
  while (container->nkind != core::ast::NodeKind::BlockStmt) {
    container = container->parent;
  }
  container->Accept([&](const core::ast::Node* vn) {
    if (vn->nkind == core::ast::NodeKind::Ident && file->ast.Text(vn) == sym_name) {
      edits.emplace_back(lsp::TextEdit{
          .range = conv::ToLSPRange(vn->nrange, file->ast),
          .newText = params.newName,
      });
    }
    return true;
  });

  return result;
}
}  // namespace vanadium::ls
