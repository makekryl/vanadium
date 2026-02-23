#include "vanadium/ls/LanguageServerTools.h"

#include <memory>

#include <vanadium/format/AstPrinter.h>
#include <vanadium/lint/Linter.h>
#include <vanadium/lint/rules/NoEmpty.h>
#include <vanadium/lint/rules/NoUnnecessaryValueof.h>
#include <vanadium/lint/rules/NoUnusedImports.h>
#include <vanadium/lint/rules/NoUnusedVars.h>
#include <vanadium/tooling/Solution.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerLogger.h"
#include "vanadium/ls/LanguageServerToolsOptions.h"

namespace vanadium::ls {

void SetupTools(LsContext& ctx) {
  // TODO: support linter configuration
  ctx.linter->RegisterRule<lint::rules::NoEmpty>();
  ctx.linter->RegisterRule<lint::rules::NoUnusedVars>();
  ctx.linter->RegisterRule<lint::rules::NoUnusedImports>();
  ctx.linter->RegisterRule<lint::rules::NoUnnecessaryValueof>();

  if (!ctx.solution) {
    return;
  }

  const auto& pmanifest = ls::tools::ReadPartialManifest(ctx.solution->RootProject());
  if (!pmanifest) {
    VLS_ERROR("Failed to read tools options: {}", pmanifest.error().String());
    return;
  }

  const auto& tc = pmanifest->tools;
  if (const auto& c = tc->fmt; c) {
    ctx.fmt_opts = std::make_unique<format::PrintOptions>();
    auto& opts = *ctx.fmt_opts;
#define COPY_OPT_IF_PRESENT(OPT)         \
  do {                                   \
    if (const auto& opt = c->OPT; opt) { \
      opts.OPT = *opt;                   \
    }                                    \
  } while (0)
    //
    COPY_OPT_IF_PRESENT(tab_width);
    COPY_OPT_IF_PRESENT(print_width);
    COPY_OPT_IF_PRESENT(max_empty_newlines);
    //
#undef COPY_OPT_IF_PRESENT
  }
}

}  // namespace vanadium::ls
