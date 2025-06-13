#include <cstdlib>
#include <mutex>
#include <print>
#include <unordered_map>

#include "JsonRpc.h"
#include "LSServer.h"
#include "LanguageServer.h"
#include "LanguageServerContext.h"
#include "LanguageServerProcedures.h"
#include "c4/yml/tag.hpp"

namespace vanadium {
namespace ls {

constexpr std::size_t kServerBacklog = 2;

namespace {
const std::unordered_map<std::string_view, VanadiumLsServer::HandlerFn> kMessageHandlers{
    {"initialize", procedures::initialize},
    {"shutdown", procedures::shutdown},
    {"exit", procedures::exit},
    {"textDocument/didOpen", procedures::textDocument::didOpen},
    {"textDocument/didChange", procedures::textDocument::didChange},
    {"textDocument/diagnostic", procedures::textDocument::diagnostic},
};

void HandleMessage(VanadiumLsContext& ctx, lserver::PooledMessageToken&& token) {
  jsonrpc::Notification notification(token->tree.rootref());

  auto it = kMessageHandlers.find(notification.method());
  if (it == kMessageHandlers.end()) {
    // TODO: log
    return;
  }

  static std::mutex m;
  std::lock_guard l(m);  // TODO: PoC
  std::println(stderr, "PROCEED {}", std::string(notification.method()));
  it->second(ctx, std::move(token));
  std::println(stderr, "COMPLETE {}", std::string(notification.method()));
}
}  // namespace

void Serve(lserver::Transport& transport, std::size_t concurrency, std::size_t jobs) {
  VanadiumLsServer server(transport, HandleMessage, concurrency, kServerBacklog);

  server.GetContext()->task_arena.initialize(jobs);

  server.Listen();
}

}  // namespace ls
}  // namespace vanadium
