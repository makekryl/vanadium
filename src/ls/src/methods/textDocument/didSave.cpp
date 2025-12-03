#include <csignal>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didSave::invoke(LsContext& ctx, const lsp::DidSaveTextDocumentParams& params) {}
}  // namespace vanadium::ls
