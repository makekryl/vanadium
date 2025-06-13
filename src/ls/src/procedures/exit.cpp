#include "JsonRpc.h"
#include "LSProtocol.h"
#include "LanguageServerProcedures.h"

namespace vanadium::ls::procedures {
void exit(VanadiumLsContext& ctx, lserver::PooledMessageToken&& token) {
  // TODO: exit gracefully
  std::exit(0);
}
}  // namespace vanadium::ls::procedures
