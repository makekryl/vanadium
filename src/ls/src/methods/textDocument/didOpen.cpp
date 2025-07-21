#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "detail/LanguageServerDiagnostic.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didOpen::operator()(LsContext& ctx, const lsp::DidOpenTextDocumentParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);

  const auto read_file = [&](std::string_view, std::string& srcbuf) {
    srcbuf = std::move(params.textDocument.text);
  };
  project.program.Commit([&](auto& modify) {
    modify.update(path, read_file);
  });

  ctx.Notify<"textDocument/publishDiagnostics">(lsp::PublishDiagnosticsParams{
      .uri = params.textDocument.uri,
      .diagnostics = detail::CollectDiagnostics(ctx, project.program, *project.program.GetFile(path)),
  });
}
}  // namespace vanadium::ls
