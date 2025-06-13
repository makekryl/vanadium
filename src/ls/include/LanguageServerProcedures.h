#pragma once

#include "LanguageServerContext.h"

namespace vanadium::ls {

// NOLINTBEGIN(readability-identifier-naming)

#define DECLARE_PROCEDURE(NAME) void NAME(VanadiumLsContext&, lserver::PooledMessageToken&&);

namespace procedures {
DECLARE_PROCEDURE(initialize);
DECLARE_PROCEDURE(shutdown);
DECLARE_PROCEDURE(exit);

namespace textDocument {
DECLARE_PROCEDURE(didOpen);
DECLARE_PROCEDURE(didChange);
DECLARE_PROCEDURE(diagnostic);
DECLARE_PROCEDURE(codeAction);
}  // namespace textDocument

}  // namespace procedures

#undef DECLARE_PROCEDURE

// NOLINTEND(readability-identifier-naming)

}  // namespace vanadium::ls
