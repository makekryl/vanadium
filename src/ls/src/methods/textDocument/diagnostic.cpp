#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::DocumentDiagnosticReport> methods::textDocument::diagnostic::operator()(
    VanadiumLsContext& ctx, const lsp::DocumentDiagnosticParams& params) {
  const auto* file = ctx->program.GetFile(std::string(params.textDocument.uri));
  if (!file) {
    throw "TODO";
  }

  lsp::RelatedFullDocumentDiagnosticReport result{.kind = "full"};

  for (const auto& err : file->ast.errors) {
    auto& item = result.items.emplace_back();
    item.source = "vanadium";
    item.severity = lsp::DiagnosticSeverity::kError;
    item.message = err.description;

    auto loc_begin = file->ast.lines.Translate(err.range.begin);
    item.range.start = {
        .line = loc_begin.line,
        .character = loc_begin.column,
    };
    auto loc_end = file->ast.lines.Translate(err.range.end);
    item.range.end = {
        .line = loc_end.line,
        .character = loc_end.column,
    };
  }
  if (file->module.has_value()) {
    for (const auto& err : file->module->unresolved) {
      auto& item = result.items.emplace_back();
      item.source = "vanadium";
      item.severity = lsp::DiagnosticSeverity::kError;
      item.message = err->On(file->ast.src);

      auto loc_begin = file->ast.lines.Translate(err->nrange.begin);
      item.range.start = {
          .line = loc_begin.line,
          .character = loc_begin.column,
      };
      auto loc_end = file->ast.lines.Translate(err->nrange.end);
      item.range.end = {
          .line = loc_end.line,
          .character = loc_end.column,
      };

      item.data = lsp::LSPAny{};
      // item.data. // TODO
    }
  }

  return result;
}
}  // namespace vanadium::ls
