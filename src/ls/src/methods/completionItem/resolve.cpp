#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "detail/Completion.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::CompletionItem> methods::completionItem::resolve::invoke(LsContext& ctx,
                                                                                  const lsp::CompletionItem& item) {
  return ctx.LockData([&](LsSessionRef d) {
    return detail::ResolveCompletionItem(item, std::move(d)).value_or(item);
  });
}
}  // namespace vanadium::ls
