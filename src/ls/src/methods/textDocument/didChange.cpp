#include <string_view>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "domain/LanguageServerDiagnostic.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didChange::operator()(LsContext& ctx, const lsp::DidChangeTextDocumentParams& params) {
  const auto& [subproject, path] = ctx->ResolveFile(params.textDocument.uri);

  const auto read_file = [&](std::string_view, vanadium::lib::Arena& arena) -> std::string_view {
    const auto new_text = std::get<lsp::TextDocumentContentChangeWholeDocument>(params.contentChanges[0]);
    return *arena.Alloc<std::string>(std::move(new_text.text));
  };
  subproject.program.Commit([&](auto& modify) {
    modify.update(path, read_file);
  });

  ctx.Notify<"textDocument/publishDiagnostics">(lsp::PublishDiagnosticsParams{
      .uri = params.textDocument.uri,
      .diagnostics = domain::CollectDiagnostics(ctx, subproject.program, *subproject.program.GetFile(path)),
  });
}
}  // namespace vanadium::ls
