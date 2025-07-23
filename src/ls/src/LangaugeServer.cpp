#include <chrono>
#include <cstdlib>
#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <glaze/util/expected.hpp>
#include <glaze/util/string_literal.hpp>
#include <mutex>

#include "BuiltinRules.h"
#include "LSConnection.h"
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
                                   methods::textDocument::didOpen,            //
                                   methods::textDocument::didChange,          //
                                   methods::textDocument::didClose,           //
                                   methods::textDocument::diagnostic,         //
                                   methods::textDocument::codeAction,         //
                                   methods::textDocument::definition,         //
                                   methods::textDocument::references,         //
                                   methods::textDocument::typeDefinition,     //
                                   methods::textDocument::hover,              //
                                   methods::textDocument::documentHighlight,  //
                                   methods::textDocument::rename,             //
                                   methods::textDocument::completion,         //
                                   methods::textDocument::inlayHint,          //
                                   //
                                   methods::completionItem::resolve,  //
                                   //
                                   methods::inlayHint::resolve  //
                                   >;                           //

template <class... Methods>
using JsonRpcServer = glz::rpc::server<typename Methods::RpcMethod...>;

template <typename Methods>
using VanadiumRpcServer = typename mp::Apply<JsonRpcServer, Methods>::type;

void Serve(lserver::Transport& transport, std::size_t concurrency, std::size_t jobs) {
  VanadiumRpcServer<ServerMethods> rpc_server;

  const auto handle_message = [&rpc_server](LsContext& ctx, lserver::PooledMessageToken&& token) {
    static std::mutex m;
    std::lock_guard l(m);  // TODO: PoC, this efficiently prevents concurrency - add RWmutexes to Program

    std::println(stderr, "PROCEED: {}", token->buf.substr(0, 64));
    const auto begin_ts = std::chrono::steady_clock::now();

    auto res_token = ctx.AcquireToken();
    ctx->task_arena.execute([&] {
      const auto results = rpc_server.call<decltype(rpc_server)::raw_call_return_t>(token->buf);
      if (results.empty()) {
        return;
      }
      assert(results.size() == 1);
      const auto& result = results.front();

      const auto ec = glz::write_json(result, res_token->buf);
      if (ec) {
        // TODO: report error
      }
    });

    if (!res_token->buf.empty()) {
      ctx.Send(std::move(res_token));
    }

    ctx->TemporaryArena().Reset();

    const auto end_ts = std::chrono::steady_clock::now();
    std::println(stderr, "  ---> Completed in {} ms",
                 std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - begin_ts).count());
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
