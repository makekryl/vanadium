#include "detail/Completion.h"

#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Program.h"
#include "detail/Definition.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::CompletionResult> methods::textDocument::completion::operator()(
    LsContext& ctx, const lsp::CompletionParams& params) {
  auto res = ctx->WithFile<lsp::CompletionResult>(
      params.textDocument.uri, [&](const core::Program&, const core::SourceFile& file) -> lsp::CompletionResult {
        const auto* n = detail::FindNode(&file, params.position);

        constexpr std::size_t kMaxCompletionItems = 120;

        std::vector<lsp::CompletionItem> items;
        detail::CollectCompletions(&file, n, ctx->TemporaryArena(), items, kMaxCompletionItems);

        return items;
      });
  return res.value_or(nullptr);
}
}  // namespace vanadium::ls
