#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <print>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "detail/Diagnostic.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didOpen::operator()(LsContext& ctx, const lsp::DidOpenTextDocumentParams& params) {
  const auto& resolution = ctx->ResolveFile(params.textDocument.uri);
  if (!resolution) {
    std::println(stderr, "'{}' does not belong to any project", params.textDocument.uri);
    return;
  }

  const auto& [project, path] = *resolution;

  const auto [it, inserted] = ctx->file_versions.try_emplace(path, params.textDocument.version);
  if (!inserted && it->second == params.textDocument.version) {
    return;
  }
  it->second = params.textDocument.version;

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
