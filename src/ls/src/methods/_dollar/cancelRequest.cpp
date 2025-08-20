#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::dollar::cancelRequest::operator()(LsContext&, const lsp::CancelParams&) {
  // TODO
}
}  // namespace vanadium::ls
