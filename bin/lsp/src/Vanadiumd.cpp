#include <unistd.h>

#include <chrono>
#include <thread>

#include "JsonRpc.h"
#include "LSMessageToken.h"
#include "LSProtocol.h"
#include "LSServer.h"
#include "LSTransport.h"

namespace {
void HandleMessage(vanadium::lsp::ServerContext<int>& ctx, vanadium::lsp::PooledMessageToken token) {
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

int main() {
  int i = 1;
  while (i == 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    usleep(100000);
  }

  vanadium::lsp::StdioTransport::Setup();
  vanadium::lsp::StdioTransport transport;
  vanadium::lsp::Server<int> server(transport, HandleMessage, 8, 2);
  server.Listen();

  return 0;
}
