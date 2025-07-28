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
#include "detail/References.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::DocumentHighlightResults> methods::textDocument::documentHighlight::operator()(
    LsContext& ctx, const lsp::DocumentHighlightParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  std::vector<lsp::DocumentHighlight> hls;
  detail::VisitLocalReferences(file, params.position, true, [&](const core::ast::nodes::Ident* ident) {
    const bool is_write = ident->parent->nkind == core::ast::NodeKind::AssignmentExpr &&
                          ident->parent->As<core::ast::nodes::AssignmentExpr>()->property == ident;
    hls.emplace_back(lsp::DocumentHighlight{
        .range = conv::ToLSPRange(ident->nrange, file->ast),
        .kind = is_write ? lsp::DocumentHighlightKind::kWrite : lsp::DocumentHighlightKind::kRead,
    });
  });

  return hls;
}
}  // namespace vanadium::ls
