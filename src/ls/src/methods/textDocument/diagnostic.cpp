#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "domain/LanguageServerDiagnostic.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::DocumentDiagnosticReport> methods::textDocument::diagnostic::operator()(
    LsContext& ctx, const lsp::DocumentDiagnosticParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);
  // for (auto& path : ctx->program.Files() | std::views::keys) {
  //   std::println(stderr, "- '{}'", path);
  // }
  if (!file) {
    std::abort();
  }

  return lsp::RelatedFullDocumentDiagnosticReport{
      .kind = "full",
      .items = domain::CollectDiagnostics(ctx, project.program, *file),
  };
}
}  // namespace vanadium::ls
