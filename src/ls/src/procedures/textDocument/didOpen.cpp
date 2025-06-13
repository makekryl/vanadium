#include "JsonRpc.h"
#include "LSProtocol.h"
#include "LanguageServerProcedures.h"

namespace vanadium::ls::procedures::textDocument {
void didOpen(VanadiumLsContext& ctx, lserver::PooledMessageToken&& token) {
  jsonrpc::Notification req(token->tree);

  lsp::DidOpenTextDocumentParams params(req.params());

  ctx->program.Commit([&](auto& modify) {
    modify.update(std::string(params.textDocument().uri()), [&](lib::Arena& arena) -> std::string_view {
      auto buf = arena.AllocStringBuffer(params.textDocument().text().length());
      std::ranges::copy(params.textDocument().text(), buf.begin());
      return {buf.data(), buf.size()};
    });
  });
}
}  // namespace vanadium::ls::procedures::textDocument
