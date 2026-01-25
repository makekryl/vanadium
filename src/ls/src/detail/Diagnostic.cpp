#include "detail/Diagnostic.h"

#include <format>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <LSProtocol.h>

#include <vanadium/core/Program.h>
#include <vanadium/lint/Context.h>
#include <vanadium/lint/Linter.h>

#include "vanadium/ls/LanguageServerConv.h"
#include "vanadium/ls/LanguageServerSession.h"
#include "vanadium/ls/detail/CodeAction.h"

namespace vanadium::ls::detail {

namespace {
void CollectModuleDiagnostics(const core::SourceFile& file, std::vector<lsp::Diagnostic>& diags, LsSessionRef& d) {
  assert(file.module.has_value());

  const auto& program = *file.program;
  const auto& module = *file.module;

  for (const auto& [import_name, import] : module.imports) {
    if (program.GetModule(import_name)) {
      continue;
    }
    diags.emplace_back(lsp::Diagnostic{
        .range = conv::ToLSPRange(import.declaration->parent->nrange, file.ast),
        .severity = lsp::DiagnosticSeverity::kError,
        .source = "vanadium",
        .message = *d.arena.Alloc<std::string>(std::format("module '{}' not found", import_name)),
    });
  }

  for (const auto& ident : module.unresolved) {
    // TODO: check fmt lib custom allocators or precalcuate size and fill string buffer manually, preferable via helper
    diags.emplace_back(lsp::Diagnostic{
        .range = conv::ToLSPRange(ident->nrange, file.ast),
        .severity = lsp::DiagnosticSeverity::kError,
        .source = "vanadium",
        .message = *d.arena.Alloc<std::string>(std::format("use of unknown symbol '{}'", ident->On(file.ast.src))),
        .data = {{
            {codeAction::kPayloadKeyUnresolved, 1},  // TODO: replace with bitmask when there will be more options
        }},
    });
    // item.data. // TODO
  }

  const auto& problems = *d.arena.Alloc<lint::ProblemSet>(d.linter.Lint(file));
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
      diags.back().data = glz::generic{
          {
              codeAction::kPayloadKeyAutofix,
              {
                  {"title", "Remove unused import"},
                  {"range", {problem.autofix->range.begin, problem.autofix->range.end}},
                  {"replacement", problem.autofix->replacement},
              },
          },
      };
    }
  }
}
}  // namespace

std::vector<lsp::Diagnostic> CollectDiagnostics(const core::SourceFile& file, LsSessionRef d) {
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
    CollectModuleDiagnostics(file, diags, d);
  }

  return diags;
}
}  // namespace vanadium::ls::detail
