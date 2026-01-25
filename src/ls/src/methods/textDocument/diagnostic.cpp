#include "vanadium/ls/detail/Diagnostic.h"

#include <LSProtocol.h>

#include <vanadium/core/Program.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/LanguageServerSession.h"

namespace vanadium::ls {
rpc::ExpectedResult<lsp::DocumentDiagnosticReport> methods::textDocument::diagnostic::invoke(
    LsContext& ctx, const lsp::DocumentDiagnosticParams& params) {
  return ctx
      .WithFile<lsp::DocumentDiagnosticReport>(params,
                                               [&](const auto&, const core::SourceFile& file, LsSessionRef d) {
                                                 return lsp::RelatedFullDocumentDiagnosticReport{
                                                     .kind = "full",
                                                     .items = detail::CollectDiagnostics(file, d),
                                                 };
                                               })
      .value_or(lsp::RelatedFullDocumentDiagnosticReport{.kind = "full"});
}
}  // namespace vanadium::ls
