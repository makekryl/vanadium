#include "detail/Diagnostic.h"

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Program.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::DocumentDiagnosticReport> methods::textDocument::diagnostic::operator()(
    LsContext& ctx, const lsp::DocumentDiagnosticParams& params) {
  auto res = ctx->WithFile<lsp::DocumentDiagnosticReport>(
      params.textDocument.uri,
      [&](const core::Program& program, const core::SourceFile& file) -> lsp::DocumentDiagnosticReport {
        return lsp::RelatedFullDocumentDiagnosticReport{
            .kind = "full",
            .items = detail::CollectDiagnostics(ctx, program, file),
        };
      });
  return res.value_or(lsp::RelatedFullDocumentDiagnosticReport{.kind = "full"});
}
}  // namespace vanadium::ls
