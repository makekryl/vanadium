#include <cstdlib>
#include <glaze/ext/jsonrpc.hpp>
#include <glaze/util/expected.hpp>
#include <glaze/util/string_literal.hpp>
#include <mutex>

#include "LSServer.h"
#include "LanguageServer.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Metaprogramming.h"

namespace vanadium {
namespace ls {

constexpr std::size_t kServerBacklog = 2;

using ServerMethods = mp::Typelist<methods::initialize,   //
                                   methods::initialized,  //
                                   methods::shutdown,     //
                                   methods::exit,         //
                                   //
                                   methods::textDocument::didOpen,     //
                                   methods::textDocument::didChange,   //
                                   methods::textDocument::diagnostic,  //
                                   //
                                   methods::workspace::diagnostic  //
                                                                   //  methods::textDocument::codeAction   //
                                   >;                              //

template <class... Methods>
using JsonRpcServer = glz::rpc::server<typename Methods::RpcMethod...>;

template <typename Methods>
using VanadiumRpcServer = typename mp::Apply<JsonRpcServer, Methods>::type;

void Serve(lserver::Transport& transport, std::size_t concurrency, std::size_t jobs) {
  VanadiumRpcServer<ServerMethods> rpc_server;

  const auto handle_message = [&rpc_server](VanadiumLsContext& ctx, lserver::PooledMessageToken&& token) {
    static std::mutex m;
    std::lock_guard l(m);  // TODO: PoC

    std::fprintf(stderr, "\n\n\n***********\nIN: ---\n%s\n---\n", token->buf.c_str());

    auto resstr = rpc_server.call(token->buf);
    std::fprintf(stderr, "OUT: ---\n%s\n---\n", resstr.c_str());

    if (resstr.empty()) {
      return;
    }

    auto res_token = ctx.AcquireToken();
    res_token->buf = std::move(resstr);
    ctx.Send(std::move(res_token));
  };

  VanadiumLsServer server(transport, handle_message, concurrency, kServerBacklog);
  auto& ctx = server.GetContext();

  {
    ServerMethods::Apply([&]<typename P>() {
      rpc_server.on<P::kMethodName>([&](const auto& params) -> glz::expected<typename P::TResult, glz::rpc::error> {
        if constexpr (std::is_same_v<typename P::TResult, std::nullptr_t>) {
          P{}(ctx, params);
          return std::nullptr_t{};
        } else {
          return P{}(ctx, params);
        }
      });
    });
  }

  ctx->task_arena.initialize(jobs);

  server.Listen();
}

}  // namespace ls
}  // namespace vanadium
