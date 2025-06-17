#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::exit::operator()(LsContext&, const rpc::Empty&) {
  // TODO: exit gracefully
  std::exit(0);
}
}  // namespace vanadium::ls
