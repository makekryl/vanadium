#include <print>
#include <ranges>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Linter.h"
#include "domain/LanguageServerDiagnostic.h"
#include "rules/NoUnusedImports.h"
#include "rules/NoUnusedVars.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::DocumentDiagnosticReport> methods::textDocument::diagnostic::operator()(
    LsContext& ctx, const lsp::DocumentDiagnosticParams& params) {
  const auto path = ctx->FileUriToPath(params.textDocument.uri);
  const auto* file = ctx->program.GetFile(path);
  // for (auto& path : ctx->program.Files() | std::views::keys) {
  //   std::println(stderr, "- '{}'", path);
  // }
  if (!file) {
    std::abort();
  }

  return lsp::RelatedFullDocumentDiagnosticReport{
      .kind = "full",
      .items = domain::CollectDiagnostics(ctx, *file),
  };
}
}  // namespace vanadium::ls
