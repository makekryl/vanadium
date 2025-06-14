#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::initialized::operator()(VanadiumLsContext&, const rpc::Empty&) {}
}  // namespace vanadium::ls
