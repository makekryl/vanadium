#include <print>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "domain/LanguageServerDiagnostic.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didChange::operator()(VanadiumLsContext& ctx,
                                                  const lsp::DidChangeTextDocumentParams& params) {
  const std::string path(params.textDocument.uri);

  ctx->program.Commit([&](auto& modify) {
    modify.update(path, [&](lib::Arena& arena) -> std::string_view {
      // TODO: move str

      const auto new_text = std::get<lsp::TextDocumentContentChangeWholeDocument>(params.contentChanges[0]);

      auto buf = arena.AllocStringBuffer(new_text.text.size());
      std::ranges::copy(new_text.text, buf.begin());
      return {buf.data(), buf.size()};
    });
  });

  ctx.Notify<"textDocument/publishDiagnostics">(lsp::PublishDiagnosticsParams{
      .uri = params.textDocument.uri,
      .diagnostics = domain::CollectDiagnostics(*ctx->program.GetFile(path)),
  });
}
}  // namespace vanadium::ls
