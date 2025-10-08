#include "LanguageServer.h"

#include <chrono>
#include <cstdlib>
#include <glaze/core/reflect.hpp>
#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <glaze/util/expected.hpp>
#include <glaze/util/string_literal.hpp>

#include "BuiltinRules.h"
#include "LSConnection.h"
#include "LanguageServerContext.h"
#include "LanguageServerLogger.h"
#include "LanguageServerMethods.h"
#include "Metaprogramming.h"

namespace vanadium::ls {

constexpr std::size_t kServerBacklog = 2;

using ServerMethods = mp::Typelist<methods::initialize,   //
                                   methods::initialized,  //
                                   methods::shutdown,     //
                                   methods::exit,         //
                                   //
                                   methods::dollar::cancelRequest,  //
                                   methods::dollar::setTrace,       //
                                   //
                                   methods::textDocument::didOpen,                //
                                   methods::textDocument::didChange,              //
                                   methods::textDocument::didSave,                //
                                   methods::textDocument::didClose,               //
                                   methods::textDocument::diagnostic,             //
                                   methods::textDocument::codeAction,             //
                                   methods::textDocument::definition,             //
                                   methods::textDocument::references,             //
                                   methods::textDocument::typeDefinition,         //
                                   methods::textDocument::hover,                  //
                                   methods::textDocument::documentHighlight,      //
                                   methods::textDocument::rename,                 //
                                   methods::textDocument::completion,             //
                                   methods::textDocument::inlayHint,              //
                                   methods::textDocument::documentSymbol,         //
                                   methods::textDocument::signatureHelp,          //
                                   methods::textDocument::semanticTokens::range,  //
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
    VLS_INFO("  |---> {}", *glz::get_as_json<std::string_view, "/method">(token->buf));
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
    VLS_INFO("   <--- ({} ms)", std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - begin_ts).count());
  };

  VanadiumLsConnection connection(handle_message, transport, concurrency, kServerBacklog);
  auto& ctx = connection.GetContext();

  {
    ServerMethods::Apply([&]<typename P>() {
      rpc_server.on<P::kMethodName>([&](const auto& params) -> glz::expected<typename P::TResult, glz::rpc::error> {
        if constexpr (std::is_same_v<typename P::TResult, rpc::Empty>) {
          P{}(ctx, params);
          return rpc::Empty{};
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

}  // namespace vanadium::ls
