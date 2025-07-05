#include <cstdlib>
#include <glaze/ext/jsonrpc.hpp>
#include <glaze/util/expected.hpp>
#include <glaze/util/string_literal.hpp>
#include <mutex>

#include "LSConnection.h"
#include "LanguageServer.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Metaprogramming.h"
#include "rules/NoEmpty.h"
#include "rules/NoUnusedImports.h"
#include "rules/NoUnusedVars.h"

namespace vanadium {
namespace ls {

constexpr std::size_t kServerBacklog = 2;

using ServerMethods = mp::Typelist<methods::initialize,   //
                                   methods::initialized,  //
                                   methods::shutdown,     //
                                   methods::exit,         //
                                   //
                                   methods::textDocument::didOpen,           //
                                   methods::textDocument::didChange,         //
                                   methods::textDocument::diagnostic,        //
                                   methods::textDocument::codeAction,        //
                                   methods::textDocument::definition,        //
                                   methods::textDocument::references,        //
                                   methods::textDocument::hover,             //
                                   methods::textDocument::documentHighlight  //
                                   >;                                        //

template <class... Methods>
using JsonRpcServer = glz::rpc::server<typename Methods::RpcMethod...>;

template <typename Methods>
using VanadiumRpcServer = typename mp::Apply<JsonRpcServer, Methods>::type;

void Serve(lserver::Transport& transport, std::size_t concurrency, std::size_t jobs) {
  VanadiumRpcServer<ServerMethods> rpc_server;

  const auto handle_message = [&rpc_server](LsContext& ctx, lserver::PooledMessageToken&& token) {
    static std::mutex m;
    std::lock_guard l(m);  // TODO: PoC, add mutexes to Program

    std::println(stderr, "PROCEED: {}", token->buf.substr(0, 128));

    std::string serialized_response;  // TODO: use buf from token
    ctx->task_arena.execute([&] {
      serialized_response = rpc_server.call(token->buf);
    });
    ctx->GetTemporaryArena().Reset();

    if (serialized_response.empty()) {
      return;
    }

    auto res_token = ctx.AcquireToken();
    res_token->buf = std::move(serialized_response);  // TODO^
    ctx.Send(std::move(res_token));
  };

  VanadiumLsConnection connection(handle_message, transport, concurrency, kServerBacklog);
  auto& ctx = connection.GetContext();

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

  ctx->linter.RegisterRule<lint::rules::NoEmpty>();
  ctx->linter.RegisterRule<lint::rules::NoUnusedVars>();
  ctx->linter.RegisterRule<lint::rules::NoUnusedImports>();

  connection.Listen();
}

}  // namespace ls
}  // namespace vanadium
