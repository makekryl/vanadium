#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"

namespace vanadium::ls {
rpc::ExpectedResult<std::nullptr_t> methods::shutdown::invoke(LsContext&, const lib::jsonrpc::Empty&) {
  return std::nullptr_t{};
}
}  // namespace vanadium::ls
