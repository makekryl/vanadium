#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::initialized::invoke(LsContext&, const rpc::Empty&) {}
}  // namespace vanadium::ls
