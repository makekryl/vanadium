#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerLogger.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "magic_enum/magic_enum.hpp"

namespace vanadium::ls {
template <>
void methods::workspace::didChangeWatchedFiles::invoke(LsContext& ctx, const lsp::DidChangeWatchedFilesParams& params) {
  ctx.LockData([&](LsSessionRef d) {
    // TODO: extract code from didOpen, didChange into module, use its methods
    for (const auto& change : params.changes) {
      VLS_INFO("change: '{}' <- {}", change.uri, magic_enum::enum_name(change.type));
    }
  });
}
}  // namespace vanadium::ls
