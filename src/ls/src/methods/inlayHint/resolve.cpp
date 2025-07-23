#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "detail/InlayHint.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::InlayHint> methods::inlayHint::resolve::operator()(LsContext& ctx,
                                                                            const lsp::InlayHint& hint) {
  if (const auto result = detail::ResolveInlayHint(*ctx->solution, ctx->TemporaryArena(), hint); result) {
    return *result;
  }
  return hint;
}
}  // namespace vanadium::ls
