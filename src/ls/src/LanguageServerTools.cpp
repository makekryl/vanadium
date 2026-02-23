#include "vanadium/ls/LanguageServerTools.h"

#include <memory>

#include <vanadium/lint/Linter.h>
#include <vanadium/lint/rules/NoEmpty.h>
#include <vanadium/lint/rules/NoUnnecessaryValueof.h>
#include <vanadium/lint/rules/NoUnusedImports.h>
#include <vanadium/lint/rules/NoUnusedVars.h>

namespace vanadium::ls {

void SetupTools(LsContext& ctx) {
  ctx.linter = std::make_unique<lint::Linter>();
  ctx.linter->RegisterRule<lint::rules::NoEmpty>();
  ctx.linter->RegisterRule<lint::rules::NoUnusedVars>();
  ctx.linter->RegisterRule<lint::rules::NoUnusedImports>();
  ctx.linter->RegisterRule<lint::rules::NoUnnecessaryValueof>();
}

}  // namespace vanadium::ls
