#include <LSProtocol.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/LanguageServerSession.h"
#include "vanadium/ls/detail/InlayHint.h"

namespace vanadium::ls {
rpc::ExpectedResult<lsp::InlayHint> methods::inlayHint::resolve::invoke(LsContext& ctx, const lsp::InlayHint& hint) {
  return ctx.LockData([&](LsSessionRef d) {
    return detail::ResolveInlayHint(hint, std::move(d)).value_or(hint);
  });
}
}  // namespace vanadium::ls
