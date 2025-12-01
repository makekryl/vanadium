#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::exit::invoke(LsContext&, const lib::jsonrpc::Empty&) {
  // TODO: exit gracefully
  std::exit(0);
}
}  // namespace vanadium::ls
