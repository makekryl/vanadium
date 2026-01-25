#include "detail/CodeAction.h"

#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
rpc::ExpectedResult<lsp::CodeActionResult> methods::textDocument::codeAction::invoke(
    LsContext& ctx, const lsp::CodeActionParams& params) {
  return ctx.WithFile<lsp::CodeActionResult>(params, detail::ProvideCodeActions).value_or(nullptr);
}
}  // namespace vanadium::ls
