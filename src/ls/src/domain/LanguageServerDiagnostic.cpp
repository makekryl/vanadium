#include "domain/LanguageServerDiagnostic.h"

#include <format>
#include <print>
#include <vector>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "Program.h"

namespace vanadium::ls::domain {

namespace {
void CollectModuleDiagnostics(LsContext& ctx, const core::SourceFile& file, std::vector<lsp::Diagnostic>& diags) {
  assert(file.module.has_value());
  const auto& module = *file.module;

  for (const auto& [import_name, import] : module.imports) {
    if (ctx->program.GetModule(import_name)) {
      continue;
    }
    const auto& message =
        *ctx->GetTemporaryArena().Alloc<std::string>(std::format("module '{}' not found", import_name));
    diags.emplace_back(lsp::Diagnostic{
        .range = conv::ToLSPRange(import.declaration->nrange, file.ast),
        .severity = lsp::DiagnosticSeverity::kError,
        .source = "vanadium",
        .message = message,
    });
  }

  for (const auto& ident : module.unresolved) {
    // TODO: check fmt lib custom allocators or precalcuate size and fill string buffer manually, preferable via helper
    const auto& message = *ctx->GetTemporaryArena().Alloc<std::string>(
        std::format("use of unknown symbol '{}'", ident->On(file.ast.src)));

    diags.emplace_back(lsp::Diagnostic{
        .range = conv::ToLSPRange(ident->nrange, file.ast),
        .severity = lsp::DiagnosticSeverity::kError,
        .source = "vanadium",
        .message = message,
    });
    // item.data. // TODO
  }

  const auto& problems = *ctx->GetTemporaryArena().Alloc<lint::ProblemSet>(ctx->linter.Lint(ctx->program, file));
  for (const auto& problem : problems) {
    diags.emplace_back(lsp::Diagnostic{
        .range = conv::ToLSPRange(problem.range, file.ast),
        .severity = lsp::DiagnosticSeverity::kWarning,  // TODO
        .code = problem.reporter,
        .source = "vanadium-tidy",
        .message = problem.description,
        .tags = std::vector<lsp::DiagnosticTag>{lsp::DiagnosticTag::kUnnecessary},  // TODO
    });
  }
}
}  // namespace

std::vector<lsp::Diagnostic> CollectDiagnostics(LsContext& ctx, const core::SourceFile& file) {
  std::vector<lsp::Diagnostic> diags;

  for (const auto& err : file.ast.errors) {
    auto& item = diags.emplace_back();
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
    CollectModuleDiagnostics(ctx, file, diags);
  }

  return diags;
}
}  // namespace vanadium::ls::domain
