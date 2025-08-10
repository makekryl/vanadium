#include "detail/Diagnostic.h"

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "Program.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::DocumentDiagnosticReport> methods::textDocument::diagnostic::operator()(
    LsContext& ctx, const lsp::DocumentDiagnosticParams& params) {
  return ctx
      ->WithFile<lsp::DocumentDiagnosticReport>(params,
                                                [&](const auto&, const core::SourceFile& file, LsSessionRef d) {
                                                  return lsp::RelatedFullDocumentDiagnosticReport{
                                                      .kind = "full",
                                                      .items = detail::CollectDiagnostics(file, d),
                                                  };
                                                })
      .value_or(lsp::RelatedFullDocumentDiagnosticReport{.kind = "full"});
}
}  // namespace vanadium::ls
