#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <print>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "domain/LanguageServerDiagnostic.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didOpen::operator()(VanadiumLsContext& ctx, const lsp::DidOpenTextDocumentParams& params) {
  const std::string path(params.textDocument.uri);

  ctx->program.Commit([&](auto& modify) {
    modify.update(path, [&](lib::Arena& arena) -> std::string_view {
      // TODO: move str
      auto buf = arena.AllocStringBuffer(params.textDocument.text.size());
      std::ranges::copy(params.textDocument.text, buf.begin());
      return {buf.data(), buf.size()};
    });
  });

  ctx.Notify<"textDocument/publishDiagnostics">(lsp::PublishDiagnosticsParams{
      .uri = params.textDocument.uri,
      .diagnostics = domain::CollectDiagnostics(*ctx->program.GetFile(path)),
  });
}
}  // namespace vanadium::ls
