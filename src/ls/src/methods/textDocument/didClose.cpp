#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "Program.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didClose::invoke(LsContext& ctx, const lsp::DidCloseTextDocumentParams& params) {
  ctx.WithFile(params, [&](const lsp::DidCloseTextDocumentParams&, const core::SourceFile& file, LsSessionRef) {
    const_cast<core::SourceFile&>(file).skip_analysis = true;
  });
}
}  // namespace vanadium::ls
