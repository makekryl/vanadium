#include "detail/InlayHint.h"

#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "detail/LanguageServerConv.h"

namespace vanadium::ls {

template <>
rpc::ExpectedResult<lsp::InlayHintResult> methods::textDocument::inlayHint::operator()(
    LsContext& ctx, const lsp::InlayHintParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  if (!file->module) {
    return nullptr;
  }

  const auto requested_range = conv::FromLSPRange(params.range, file->ast);
  return detail::CollectInlayHints(file, requested_range, ctx->TemporaryArena());
}
}  // namespace vanadium::ls
