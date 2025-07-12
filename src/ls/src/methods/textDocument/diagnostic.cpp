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
  const auto& [subproject, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = subproject.program.GetFile(path);
  // for (auto& path : ctx->program.Files() | std::views::keys) {
  //   std::println(stderr, "- '{}'", path);
  // }
  if (!file) {
    std::abort();
  }

  return lsp::RelatedFullDocumentDiagnosticReport{
      .kind = "full",
      .items = domain::CollectDiagnostics(ctx, subproject.program, *file),
  };
}
}  // namespace vanadium::ls
