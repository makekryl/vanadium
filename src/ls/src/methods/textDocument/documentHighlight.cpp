#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/core/Semantic.h>

#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "detail/References.h"

namespace vanadium::ls {
namespace {
lsp::DocumentHighlightResults ProvideHighlight(const lsp::DocumentHighlightParams& params, const core::SourceFile& file,
                                               LsSessionRef) {
  std::vector<lsp::DocumentHighlight> hls;

  detail::VisitLocalReferences(&file, params.position, true, [&](const ast::nodes::Ident* ident) {
    const bool is_write = ident->parent->nkind == ast::NodeKind::AssignmentExpr &&
                          ident->parent->As<ast::nodes::AssignmentExpr>()->property == ident;
    hls.emplace_back(lsp::DocumentHighlight{
        .range = conv::ToLSPRange(ident->nrange, file.ast),
        .kind = is_write ? lsp::DocumentHighlightKind::kWrite : lsp::DocumentHighlightKind::kRead,
    });
  });

  return hls;
}
}  // namespace

rpc::ExpectedResult<lsp::DocumentHighlightResults> methods::textDocument::documentHighlight::invoke(
    LsContext& ctx, const lsp::DocumentHighlightParams& params) {
  return ctx.WithFile<lsp::DocumentHighlightResults>(params, ProvideHighlight).value_or(nullptr);
}
}  // namespace vanadium::ls
