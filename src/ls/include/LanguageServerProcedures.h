#pragma once

#include "LanguageServerContext.h"

namespace vanadium::ls {
// NOLINTBEGIN(readability-identifier-naming)

namespace procedures {
void initialize(VanadiumLsContext&, lserver::PooledMessageToken&&);
void shutdown(VanadiumLsContext&, lserver::PooledMessageToken&&);
void exit(VanadiumLsContext&, lserver::PooledMessageToken&&);

namespace textDocument {
void didOpen(VanadiumLsContext&, lserver::PooledMessageToken&&);
void didChange(VanadiumLsContext&, lserver::PooledMessageToken&&);
void diagnostic(VanadiumLsContext&, lserver::PooledMessageToken&&);
}  // namespace textDocument

}  // namespace procedures

// NOLINTEND(readability-identifier-naming)

}  // namespace vanadium::ls
