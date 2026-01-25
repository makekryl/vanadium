#include <LSProtocol.h>
#include <vanadium/core/Program.h>

#include <string_view>
#include <variant>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerConv.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/LanguageServerSession.h"
#include "vanadium/ls/detail/Diagnostic.h"

namespace vanadium::ls {
void methods::textDocument::didChange::invoke(LsContext& ctx, const lsp::DidChangeTextDocumentParams& params) {
  ctx.WithFile(params, [&](const auto&, const core::SourceFile& file, LsSessionRef d) {
    ctx.file_versions[file.path] = params.textDocument.version;

    const auto read_file = [&](std::string_view, std::string& srcbuf) -> void {
      if (params.contentChanges.size() == 1 &&
          std::holds_alternative<lsp::TextDocumentContentChangeWholeDocument>(params.contentChanges.front())) {
        const auto& revision = std::get<lsp::TextDocumentContentChangeWholeDocument>(params.contentChanges.front());
        srcbuf = std::move(revision.text);
        return;
      }

      for (const auto& v : params.contentChanges) {
        assert(std::holds_alternative<lsp::TextDocumentContentChangePartial>(v));

        const auto& change = std::get<lsp::TextDocumentContentChangePartial>(v);

        const auto range = conv::FromLSPRange(change.range, file.ast);
        srcbuf.replace(range.begin, range.Length(), change.text);
      }
    };
    file.program->Commit([&](auto& modify) {
      modify.update(file.path, read_file);
    });

    ctx.connection->Notify<"textDocument/publishDiagnostics">(lsp::PublishDiagnosticsParams{
        .uri = params.textDocument.uri,
        .version = params.textDocument.version,
        .diagnostics = detail::CollectDiagnostics(file, d),
    });
  });
}
}  // namespace vanadium::ls
