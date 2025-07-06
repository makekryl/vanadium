#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didClose::operator()(LsContext& ctx, const lsp::DidCloseTextDocumentParams& params) {}
}  // namespace vanadium::ls
