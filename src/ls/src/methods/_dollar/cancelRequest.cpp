#include <LSProtocol.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"

namespace vanadium::ls {
void methods::dollar::cancelRequest::invoke(LsContext&, const lsp::CancelParams&) {
  // TODO
}
}  // namespace vanadium::ls
