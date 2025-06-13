#include "JsonRpc.h"
#include "LSProtocol.h"
#include "LanguageServerProcedures.h"

namespace vanadium::ls::procedures::textDocument {
void didChange(VanadiumLsContext& ctx, lserver::PooledMessageToken&& token) {
  jsonrpc::Notification req(token->tree);

  lsp::DidChangeTextDocumentParams params(req.params());

  ctx->program.Commit([&](auto& modify) {
    modify.update(std::string(params.textDocument().uri()), [&](lib::Arena& arena) -> std::string_view {
      const auto new_text = params.contentChanges().jsonNode().child(0)["text"].val();

      auto buf = arena.AllocStringBuffer(new_text.size());
      std::ranges::copy(new_text, buf.begin());
      return {buf.data(), buf.size()};
    });
  });
}
}  // namespace vanadium::ls::procedures::textDocument
