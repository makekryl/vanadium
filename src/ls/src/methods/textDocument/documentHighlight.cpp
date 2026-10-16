#include <LSProtocol.h>
#include <LSProtocolEx.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/core/Semantic.h>

#include "vanadium/core/Program.h"
#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerConv.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/LanguageServerSession.h"
#include "vanadium/ls/detail/References.h"

namespace vanadium::ls {
namespace {
lsp::DocumentHighlightResults ProvideHighlight(const lsp::DocumentHighlightParams& params, const core::SourceFile& file,
                                               LsSessionRef) {
  const auto symres = detail::FindSymbol(&file, params.position);
  if (!symres) {
    return {};
  }

  std::vector<lsp::DocumentHighlight> hls;
  detail::VisitReferences(
      &file, *symres,
      detail::ReferencesVisitParams{
          .global_search = false,
          .include_declaration = true,
      },
      [&](const core::SourceFile*, const ast::Node* m) {
        const bool is_write = m->parent->nkind == ast::NodeKind::AssignmentExpr &&
                              m->parent->As<ast::nodes::AssignmentExpr>()->property == m;
        hls.emplace_back(lsp::DocumentHighlight{
            .range = conv::ToLSPRange(m->nrange, file.ast),
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
