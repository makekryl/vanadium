#include <LSProtocol.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/detail/Completion.h"

namespace vanadium::ls {
rpc::ExpectedResult<lsp::CompletionItem> methods::completionItem::resolve::invoke(LsContext& ctx,
                                                                                  const lsp::CompletionItem& item) {
  return ctx.LockData([&](LsSessionRef d) {
    return detail::ResolveCompletionItem(item, std::move(d)).value_or(item);
  });
}
}  // namespace vanadium::ls
