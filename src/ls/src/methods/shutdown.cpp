#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<std::nullptr_t> methods::shutdown::operator()(LsContext&, const rpc::Empty&) {
  return std::nullptr_t{};
}
}  // namespace vanadium::ls
