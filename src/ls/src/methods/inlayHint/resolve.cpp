#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "detail/InlayHint.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::InlayHint> methods::inlayHint::resolve::operator()(LsContext& ctx,
                                                                            const lsp::InlayHint& hint) {
  return ctx->LockData([&](LsSessionRef d) {
    return detail::ResolveInlayHint(hint, std::move(d)).value_or(hint);
  });
}
}  // namespace vanadium::ls
