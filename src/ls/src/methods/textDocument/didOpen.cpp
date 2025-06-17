#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <print>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "domain/LanguageServerDiagnostic.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didOpen::operator()(LsContext& ctx, const lsp::DidOpenTextDocumentParams& params) {
  const auto path = ctx->FileUriToPath(params.textDocument.uri);

  const auto read_file = [&](std::string_view, vanadium::lib::Arena& arena) -> std::string_view {
    // TODO: move str
    auto buf = arena.AllocStringBuffer(params.textDocument.text.size());
    std::ranges::copy(params.textDocument.text, buf.begin());
    return {buf.data(), buf.size()};
  };
  ctx->program.Commit([&](auto& modify) {
    modify.update(path, read_file);
  });

  ctx.Notify<"textDocument/publishDiagnostics">(lsp::PublishDiagnosticsParams{
      .uri = params.textDocument.uri,
      .diagnostics = domain::CollectDiagnostics(ctx, *ctx->program.GetFile(path)),
  });
}
}  // namespace vanadium::ls
