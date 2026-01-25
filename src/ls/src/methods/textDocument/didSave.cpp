#include <csignal>

#include <LSProtocol.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"

namespace vanadium::ls {
void methods::textDocument::didSave::invoke(LsContext&, const lsp::DidSaveTextDocumentParams&) {}
}  // namespace vanadium::ls
