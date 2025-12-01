#include "detail/Completion.h"

#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::CompletionResult> methods::textDocument::completion::invoke(
    LsContext& ctx, const lsp::CompletionParams& params) {
  return ctx.WithFile<lsp::CompletionResult>(params, detail::CollectCompletions).value_or(nullptr);
}
}  // namespace vanadium::ls
