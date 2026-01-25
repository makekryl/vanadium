#include <LSProtocol.h>

#include <csignal>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"

namespace vanadium::ls {
void methods::textDocument::didSave::invoke(LsContext& ctx, const lsp::DidSaveTextDocumentParams& params) {}
}  // namespace vanadium::ls
