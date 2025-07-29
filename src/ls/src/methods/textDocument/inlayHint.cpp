#include "detail/InlayHint.h"

#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "TypeChecker.h"

namespace vanadium::ls {

template <>
rpc::ExpectedResult<lsp::InlayHintResult> methods::textDocument::inlayHint::operator()(
    LsContext& ctx, const lsp::InlayHintParams& params) {
  auto res = ctx->WithFile<lsp::InlayHintResult>(
      params.textDocument.uri, [&](const core::Program&, const core::SourceFile& file) -> lsp::InlayHintResult {
        if (!file.module) {
          return nullptr;
        }

        const auto requested_range = conv::FromLSPRange(params.range, file.ast);
        return detail::CollectInlayHints(&file, requested_range, ctx->TemporaryArena());
      });
  return res.value_or(nullptr);
}
}  // namespace vanadium::ls
