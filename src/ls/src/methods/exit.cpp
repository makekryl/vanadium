#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"

namespace vanadium::ls {
void methods::exit::invoke(LsContext&, const lib::jsonrpc::Empty&) {
  // TODO: exit gracefully
  std::exit(0);
}
}  // namespace vanadium::ls
