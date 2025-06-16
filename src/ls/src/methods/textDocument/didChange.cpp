#include <print>
#include <string_view>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "domain/LanguageServerDiagnostic.h"

static std::string lsptoreal(std::string_view lsppath) {
  constexpr std::size_t prefixsize = std::string_view{"file://"}.size();
  return std::string(lsppath.substr(prefixsize, lsppath.size() - prefixsize));
}

namespace vanadium::ls {
template <>
void methods::textDocument::didChange::operator()(VanadiumLsContext& ctx,
                                                  const lsp::DidChangeTextDocumentParams& params) {
  const auto path = ctx->project->RelativizePath(lsptoreal(std::string(params.textDocument.uri)));

  const auto read_file = [&](std::string_view, vanadium::lib::Arena& arena) -> std::string_view {
    // TODO: move str

    const auto new_text = std::get<lsp::TextDocumentContentChangeWholeDocument>(params.contentChanges[0]);

    auto buf = arena.AllocStringBuffer(new_text.text.size());
    std::ranges::copy(new_text.text, buf.begin());
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
