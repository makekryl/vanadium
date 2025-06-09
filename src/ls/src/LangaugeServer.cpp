#include "JsonRpc.h"
#include "LSProtocol.h"
#include "LSServer.h"
#include "LanguageServerContext.h"

namespace vanadium {
namespace ls {

constexpr std::size_t kServerBacklog = 2;

namespace {
void HandleMessage(VanadiumLsContext& ctx, lserver::PooledMessageToken token) {
  jsonrpc::Notification notification(token->tree.rootref());
  if (notification.method() != "initialize") {
    return;
  }
  jsonrpc::Request req(notification);

  auto res_token = ctx.AcquireToken();

  auto res = jsonrpc::Response::Success(res_token->tree.rootref(), req.id());

  lsp::InitializeResult ir(res.result());
  ir.init();

  // ir.capabilities().add_hoverProvider().jsonNode().set_type(ryml::VAL);
  // ir.capabilities().hoverProvider().set_boolean(true);

  ir.add_serverInfo().set_name("vanadiumd");
  ir.serverInfo().set_version("0.0.0.1");

  ctx.Send(std::move(res_token));
}
}  // namespace

void Serve(lserver::Transport& transport, std::size_t concurrency, std::size_t jobs) {
  lserver::Server<VanadiumLsState> server(transport, HandleMessage, concurrency, kServerBacklog);

  server.GetContext()->task_arena.initialize(jobs);

  server.Listen();
}

}  // namespace ls
}  // namespace vanadium
