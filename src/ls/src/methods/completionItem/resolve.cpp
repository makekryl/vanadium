#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "detail/LanguageServerCompletion.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::CompletionResolutionResult> methods::completionItem::resolve::operator()(
    LsContext& ctx, const lsp::CompletionItem& item) {
  if (const auto result = detail::ResolveCompletionItem(*ctx->solution, ctx->GetTemporaryArena(), item); result) {
    return *result;
  }
  return nullptr;
}
}  // namespace vanadium::ls
