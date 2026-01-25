#include "detail/InlayHint.h"

#include <LSProtocol.h>
#include <LSProtocolEx.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"

namespace vanadium::ls {

rpc::ExpectedResult<lsp::InlayHintResult> methods::textDocument::inlayHint::invoke(LsContext& ctx,
                                                                                   const lsp::InlayHintParams& params) {
  return ctx.WithFile<lsp::InlayHintResult>(params, detail::CollectInlayHints).value_or(nullptr);
}
}  // namespace vanadium::ls
