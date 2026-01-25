#include <LSProtocol.h>

#include <vanadium/core/Program.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/LanguageServerSession.h"

namespace vanadium::ls {
void methods::textDocument::didClose::invoke(LsContext& ctx, const lsp::DidCloseTextDocumentParams& params) {
  ctx.WithFile(params, [&](const lsp::DidCloseTextDocumentParams&, const core::SourceFile& file, LsSessionRef) {
    const_cast<core::SourceFile&>(file).skip_analysis = true;
  });
}
}  // namespace vanadium::ls
