#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::dollar::setTrace::invoke(LsContext&, const lsp::SetTraceParams&) {
  // TODO
}
}  // namespace vanadium::ls
