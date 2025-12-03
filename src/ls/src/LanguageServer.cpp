#include "LanguageServer.h"

#include <vanadium/lib/jsonrpc/Server.h>
#include <vanadium/lib/lserver/Connection.h>

#include <chrono>
#include <cstdlib>
#include <glaze/json.hpp>

#include "BuiltinRules.h"
#include "LanguageServerContext.h"
#include "LanguageServerLogger.h"
#include "LanguageServerMethods.h"
#include "Metaprogramming.h"

namespace vanadium::ls {

constexpr std::size_t kServerBacklog = 8;

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
                                   methods::workspace::didChangeWatchedFiles,  //
                                   //
                                   methods::completionItem::resolve,  //
                                   //
                                   methods::inlayHint::resolve  //
                                   >;                           //

void Serve(lserver::Transport& transport, std::size_t concurrency, std::size_t jobs) {
  lib::jsonrpc::Server<LsContext> rpc_server;
  std::optional<LsContext> ctx;

  const auto handle_message = [&rpc_server, &ctx](lserver::Connection& conn, lserver::PooledMessageToken&& token) {
    const auto method = glz::get_as_json<std::string_view, "/method">(token->buf);
    if (!method) [[unlikely]] {
      VLS_WARN("  |---> Malformed message or message with unset method has been received");
      VLS_DEBUG("       {}", glz::format_error(method.error(), token->buf));
      return;
    }

    if (!rpc_server.IsBound(*method)) [[unlikely]] {
      VLS_WARN("  |---> Method '{}' is not supported", *method);
      return;
    }

    VLS_INFO("  |---> {}", *method);
    const auto begin_ts = std::chrono::steady_clock::now();

    auto res_token = conn.AcquireToken();
    ctx->task_arena.execute([&] {
      rpc_server.Call(*ctx, res_token->buf, token->buf);
    });

    if (!res_token->buf.empty()) {
      conn.Send(std::move(res_token));
    }

    ctx->TemporaryArena().Reset();

    const auto end_ts = std::chrono::steady_clock::now();
    VLS_INFO("   <--- ({} ms)", std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - begin_ts).count());
  };

  lserver::Connection connection(handle_message, transport, concurrency, kServerBacklog);
  ctx.emplace(connection);

  {
    ServerMethods::Apply([&]<typename P>() {
      rpc_server.Bind<P::invoke>(P::kMethodName);
    });
  }

  ctx->task_arena.initialize(jobs);

  ctx->linter.RegisterRule<lint::rules::NoEmpty>();
  ctx->linter.RegisterRule<lint::rules::NoUnusedVars>();
  ctx->linter.RegisterRule<lint::rules::NoUnusedImports>();

  connection.Listen();
}

}  // namespace vanadium::ls
