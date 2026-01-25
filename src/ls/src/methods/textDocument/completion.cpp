#include "vanadium/ls/detail/Completion.h"

#include <LSProtocol.h>
#include <LSProtocolEx.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"

namespace vanadium::ls {
rpc::ExpectedResult<lsp::CompletionResult> methods::textDocument::completion::invoke(
    LsContext& ctx, const lsp::CompletionParams& params) {
  return ctx.WithFile<lsp::CompletionResult>(params, detail::CollectCompletions).value_or(nullptr);
}
}  // namespace vanadium::ls
