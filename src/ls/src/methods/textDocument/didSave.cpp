#include <csignal>
#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didSave::invoke(LsContext& ctx, const lsp::DidSaveTextDocumentParams& params) {
  std::raise(SIGSEGV);
}
}  // namespace vanadium::ls
