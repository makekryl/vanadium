#include <print>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::WorkspaceDiagnosticReport> methods::workspace::diagnostic::operator()(
    VanadiumLsContext& ctx, const lsp::WorkspaceDiagnosticParams& params) {
  lsp::WorkspaceDiagnosticReport result;

  for (const auto& [path, file] : ctx->program.Files()) {
    std::println(stderr, "path={}", path);

    auto& file_report = std::get<lsp::WorkspaceFullDocumentDiagnosticReport>(
        result.items.emplace_back(lsp::WorkspaceFullDocumentDiagnosticReport{.kind = "full"}));

    file_report.uri = path;
    file_report.version = 1;

    // file_report.items = domain::
  }

  return result;
}
}  // namespace vanadium::ls
