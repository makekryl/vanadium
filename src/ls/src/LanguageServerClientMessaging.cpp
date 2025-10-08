#include "LanguageServerClientMessaging.h"

#include "LSProtocolEx.h"

namespace vanadium::ls::clientMessaging {

lsp::ShowMessageRequestResult ShowMessage(LsContext& ctx, lsp::ShowMessageRequestParams&& params) {
  if (params.actions->empty()) {
    // TODO: remove this special case, the problem is that we cannot receive the response before
    // initialization request is completed, causing a deadlock
    ctx.Notify<"window/showMessage">(std::move(params));
    return nullptr;
  }

  auto res = ctx.Request<"window/showMessage", lsp::ShowMessageRequestResult>(std::move(params));
  if (res.error()) [[unlikely]] {
    // points for consideration:
    // - those errors can be only json (de)serialization errors
    // - generally, we don't need to distinguish between actual error and nully result here
    // - on the other hand, returning nullptr may not work out for other server->client requests,
    //   as their results not necessary actually nullable
    // - exposing such low-level encoding error to high-level logic is evil
    // - wrapping it as std::optional<lsp::ShowMessageRequestResult>,
    //   i.e. std::optional<std::variant<ActualResult, std::nullptr_t>> is terrible
    return nullptr;
  }
  return *res;
}

}  // namespace vanadium::ls::clientMessaging
