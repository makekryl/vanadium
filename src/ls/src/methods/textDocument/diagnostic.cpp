#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "domain/LanguageServerDiagnostic.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::DocumentDiagnosticReport> methods::textDocument::diagnostic::operator()(
    VanadiumLsContext& ctx, const lsp::DocumentDiagnosticParams& params) {
  const auto* file = ctx->program.GetFile(std::string(params.textDocument.uri));
  if (!file) {
    throw "TODO";
  }

  return lsp::RelatedFullDocumentDiagnosticReport{
      .kind = "full",
      .items = domain::CollectDiagnostics(*file),
  };
}
}  // namespace vanadium::ls
