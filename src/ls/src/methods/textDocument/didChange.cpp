#include <string_view>
#include <variant>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "detail/LanguageServerDiagnostic.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didChange::operator()(LsContext& ctx, const lsp::DidChangeTextDocumentParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);

  const auto read_file = [&](std::string_view, std::string& srcbuf) -> void {
    if (params.contentChanges.size() == 1 &&
        std::holds_alternative<lsp::TextDocumentContentChangeWholeDocument>(params.contentChanges.front())) {
      const auto& revision = std::get<lsp::TextDocumentContentChangeWholeDocument>(params.contentChanges.front());
      srcbuf = std::move(revision.text);
      return;
    }

    const auto* sf = project.program.GetFile(path);

    for (const auto& v : params.contentChanges) {
      assert(std::holds_alternative<lsp::TextDocumentContentChangePartial>(v));

      const auto& change = std::get<lsp::TextDocumentContentChangePartial>(params.contentChanges.front());

      const auto range = conv::FromLSPRange(change.range, sf->ast);
      srcbuf.replace(range.begin, range.Length(), change.text);
    }
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
