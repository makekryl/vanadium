#include <LSProtocol.h>
#include <vanadium/core/Program.h>

#include <utility>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerLogger.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/LanguageServerSession.h"
#include "vanadium/ls/detail/Diagnostic.h"

namespace vanadium::ls {
void methods::textDocument::didOpen::invoke(LsContext& ctx, const lsp::DidOpenTextDocumentParams& params) {
  ctx.LockData([&](LsSessionRef d) {
    const auto& resolution = ctx.ResolveFileUri(params.textDocument.uri);
    if (!resolution) {
      VLS_ERROR("File '{}' does not belong to any project", params.textDocument.uri);
      return;
    }

    const auto& [project, path] = *resolution;
    VLS_DEBUG("didOpen({})", path);

    const auto [it, inserted] = ctx.file_versions.try_emplace(path, params.textDocument.version);
    if (inserted || it->second != params.textDocument.version) {
      if (const auto* sf = project.program.GetFile(path); sf) {
        VLS_DEBUG("didOpen({}): analysis={:b}, skipped={}, ver={}->{}", path, std::to_underlying(sf->analysis_state),
                  sf->skip_analysis, it->second, params.textDocument.version);
      }

      it->second = params.textDocument.version;

      const auto read_file = [&](std::string_view, std::string& srcbuf) {
        srcbuf = std::move(params.textDocument.text);
      };
      project.program.Update([&](auto& modify) {
        modify.update(path, read_file);
      });
    }

    const_cast<core::SourceFile*>(project.program.GetFile(path))->skip_analysis = false;
    project.program.Commit([](auto&) {});

    ctx.connection->Notify<"textDocument/publishDiagnostics">(lsp::PublishDiagnosticsParams{
        .uri = params.textDocument.uri,
        .version = params.textDocument.version,
        .diagnostics = detail::CollectDiagnostics(*project.program.GetFile(path), d),
    });
  });
}
}  // namespace vanadium::ls
