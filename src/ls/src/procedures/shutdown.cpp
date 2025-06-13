#include "JsonRpc.h"
#include "LSProtocol.h"
#include "LanguageServerProcedures.h"

namespace vanadium::ls::procedures {
void shutdown(VanadiumLsContext& ctx, lserver::PooledMessageToken&& token) {
  jsonrpc::Request req(token->tree);

  auto res = ctx.AcquireToken();
  auto jsonrpcres = jsonrpc::Response::Success(res->tree, req.id());
  jsonrpcres.result() << nullptr;

  ctx.Send(std::move(res));
}
}  // namespace vanadium::ls::procedures
