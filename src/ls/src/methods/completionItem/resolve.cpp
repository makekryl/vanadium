#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "detail/Completion.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::CompletionItem> methods::completionItem::resolve::operator()(LsContext& ctx,
                                                                                      const lsp::CompletionItem& item) {
  if (const auto result = detail::ResolveCompletionItem(*ctx->solution, ctx->TemporaryArena(), item); result) {
    return *result;
  }
  return item;
}
}  // namespace vanadium::ls
