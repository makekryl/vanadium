#include "domain/LanguageServerDiagnostic.h"

#include <print>
#include <vector>

#include "LSProtocol.h"

namespace vanadium::ls::domain {
std::vector<lsp::Diagnostic> CollectDiagnostics(const core::SourceFile& file) {
  std::vector<lsp::Diagnostic> result;

  for (const auto& err : file.ast.errors) {
    auto& item = result.emplace_back();
    item.source = "vanadium";
    item.severity = lsp::DiagnosticSeverity::kError;
    item.message = err.description;

    auto loc_begin = file.ast.lines.Translate(err.range.begin);
    item.range.start = {
        .line = loc_begin.line,
        .character = loc_begin.column,
    };
    auto loc_end = file.ast.lines.Translate(err.range.end);
    item.range.end = {
        .line = loc_end.line,
        .character = loc_end.column,
    };
  }
  if (file.module.has_value()) {
    std::println(stderr, "collect diag for {}, unresolved.size = {}", file.path, file.module->unresolved.size());
    for (const auto& err : file.module->unresolved) {
      auto& item = result.emplace_back();
      item.source = "vanadium";
      item.severity = lsp::DiagnosticSeverity::kError;
      item.message = err->On(file.ast.src);

      auto loc_begin = file.ast.lines.Translate(err->nrange.begin);
      item.range.start = {
          .line = loc_begin.line,
          .character = loc_begin.column,
      };
      auto loc_end = file.ast.lines.Translate(err->nrange.end);
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
}  // namespace vanadium::ls::domain
