#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Program.h"
#include "detail/LanguageServerCompletion.h"
#include "detail/LanguageServerSymbolDef.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::CompletionResult> methods::textDocument::completion::operator()(
    LsContext& ctx, const lsp::CompletionParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  const auto* n = detail::FindNode(file, params.position);

  constexpr std::size_t kMaxCompletionItems = 80;

  std::vector<lsp::CompletionItem> items;
  detail::CollectCompletions(file, n, ctx->GetTemporaryArena(), items, kMaxCompletionItems);
  return items;
}
}  // namespace vanadium::ls
