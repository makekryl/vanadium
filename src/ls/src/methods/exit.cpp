#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::exit::operator()(VanadiumLsContext&, const rpc::Empty&) {
  // TODO: exit gracefully
  std::exit(0);
}
}  // namespace vanadium::ls
