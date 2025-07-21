#include "detail/LanguageServerDiagnostic.h"

#include <format>
#include <vector>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "Program.h"
#include "magic_enum/magic_enum.hpp"

namespace vanadium::ls::detail {

namespace {
void CollectModuleDiagnostics(LsContext& ctx, const core::Program& program, const core::SourceFile& file,
                              std::vector<lsp::Diagnostic>& diags) {
  assert(file.module.has_value());
  const auto& module = *file.module;

  for (const auto& [import_name, import] : module.imports) {
    if (program.GetModule(import_name)) {
      continue;
    }
    const auto& message = ctx->Temp<std::string>(std::format("module '{}' not found", import_name));
    diags.emplace_back(lsp::Diagnostic{
        .range = conv::ToLSPRange(import.declaration->nrange, file.ast),
        .severity = lsp::DiagnosticSeverity::kError,
        .source = "vanadium",
        .message = message,
    });
  }

  for (const auto& ident : module.unresolved) {
    // TODO: check fmt lib custom allocators or precalcuate size and fill string buffer manually, preferable via helper
    const auto& message = ctx->Temp<std::string>(std::format("use of unknown symbol '{}'", ident->On(file.ast.src)));

    diags.emplace_back(lsp::Diagnostic{
        .range = conv::ToLSPRange(ident->nrange, file.ast),
        .severity = lsp::DiagnosticSeverity::kError,
        .source = "vanadium",
        .message = message,
        .data = {{
            {"unresolved", 1},
        }},
    });
    // item.data. // TODO
  }

  const auto& problems = ctx->Temp<lint::ProblemSet>(ctx->linter.Lint(program, file));
  for (const auto& problem : problems) {
    diags.emplace_back(lsp::Diagnostic{
        .range = conv::ToLSPRange(problem.range, file.ast),
        .severity = lsp::DiagnosticSeverity::kWarning,  // TODO
        .code = problem.reporter,
        .source = "vanadium-tidy",
        .message = problem.description,
        .tags = std::vector<lsp::DiagnosticTag>{lsp::DiagnosticTag::kUnnecessary},  // TODO
    });
    if (problem.autofix) {
      diags.back().data = glz::json_t{
          {
              "autofix",
              {
                  {"title", "Remove unused import"},
                  {"range", {problem.autofix->range.begin, problem.autofix->range.end}},
              },
          },
      };
    }
  }
}
}  // namespace

std::vector<lsp::Diagnostic> CollectDiagnostics(LsContext& ctx, const core::Program& program,
                                                const core::SourceFile& file) {
  std::vector<lsp::Diagnostic> diags;  // TODO: preallocate memory
  diags.reserve(256);                  // this is not real

  for (const auto& err : file.ast.errors) {
    diags.emplace_back(lsp::Diagnostic{
        .range = conv::ToLSPRange(err.range, file.ast),
        .severity = lsp::DiagnosticSeverity::kError,
        .code = "syntax",
        .source = "vanadium",
        .message = err.description,
    });
  }

  for (const auto& err : file.semantic_errors) {
    diags.emplace_back(lsp::Diagnostic{
        .range = conv::ToLSPRange(err.range, file.ast),
        .severity = lsp::DiagnosticSeverity::kError,
        .code = "semantic",
        .source = "vanadium",
        .message = magic_enum::enum_name(err.type),  // TODO
    });
  }

  for (const auto& err : file.type_errors) {
    diags.emplace_back(lsp::Diagnostic{
        .range = conv::ToLSPRange(err.range, file.ast),
        .severity = lsp::DiagnosticSeverity::kError,
        .code = "typechecker",
        .source = "vanadium",
        .message = err.message,  // TODO
    });
  }

  if (file.module.has_value()) {
    CollectModuleDiagnostics(ctx, program, file, diags);
  }

  return diags;
}
}  // namespace vanadium::ls::detail
