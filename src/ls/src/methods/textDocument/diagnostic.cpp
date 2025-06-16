#include <print>
#include <ranges>

#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Linter.h"
#include "domain/LanguageServerDiagnostic.h"
#include "rules/NoUnusedImports.h"
#include "rules/NoUnusedVars.h"

static std::string lsptoreal(const std::string& lsppath) {
  constexpr std::size_t prefixsize = std::string_view{"file://"}.size();
  return lsppath.substr(prefixsize, lsppath.size() - prefixsize);
}

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::DocumentDiagnosticReport> methods::textDocument::diagnostic::operator()(
    VanadiumLsContext& ctx, const lsp::DocumentDiagnosticParams& params) {
  const auto* file =
      ctx->program.GetFile(ctx->project->RelativizePath(lsptoreal(std::string(params.textDocument.uri))));
  for (auto& path : ctx->program.Files() | std::views::keys) {
    std::println(stderr, "- '{}'", path);
  }
  if (!file) {
    std::abort();
  }

  auto diags = domain::CollectDiagnostics(*file);

  for (const auto& [import_name, import] : file->module->imports) {
    if (ctx->program.GetModule(import_name)) {
      continue;
    }
    auto& item = diags.emplace_back();
    item.source = "vanadium";
    item.severity = lsp::DiagnosticSeverity::kWarning;
    item.message = "inaccessible import";

    auto loc_begin = file->ast.lines.Translate(import.declaration->nrange.begin);
    item.range.start = {
        .line = loc_begin.line,
        .character = loc_begin.column,
    };
    auto loc_end = file->ast.lines.Translate(import.declaration->nrange.end);
    item.range.end = {
        .line = loc_end.line,
        .character = loc_end.column,
    };
  }

  auto& problems = *ctx->GetTemporaryArena().Alloc<lint::ProblemSet>(ctx->linter.Lint(ctx->program, *file));

  for (const auto& problem : problems) {
    auto& item = diags.emplace_back();
    item.source = "vanadium";
    item.severity = lsp::DiagnosticSeverity::kWarning;
    item.message = problem.description;
    item.tags = std::vector<lsp::DiagnosticTag>{lsp::DiagnosticTag::kUnnecessary};

    auto loc_begin = file->ast.lines.Translate(problem.range.begin);
    item.range.start = {
        .line = loc_begin.line,
        .character = loc_begin.column,
    };
    auto loc_end = file->ast.lines.Translate(problem.range.end);
    item.range.end = {
        .line = loc_end.line,
        .character = loc_end.column,
    };
  }

  return lsp::RelatedFullDocumentDiagnosticReport{
      .kind = "full",
      .items = std::move(diags),
  };
}
}  // namespace vanadium::ls
