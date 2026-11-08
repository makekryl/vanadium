#include "vanadium/ls/actions/UnresolvedSymbolLookup.h"

#include <string_view>

#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Program.h>
#include <vanadium/lib/ArenaString.h>
#include <vanadium/lib/FunctionRef.h>
#include <vanadium/lib/Metaprogramming.h>

#include "vanadium/ls/LanguageServerConv.h"
#include "vanadium/ls/detail/CodeAction.h"
#include "vanadium/ls/detail/Helpers.h"

namespace vanadium::ls::actions {

namespace {
void LookupMissingImport(const core::Program& program, std::string_view symbol_name,
                         const mp::Consumer<const core::ModuleDescriptor&> auto& accept_candidate) {
  program.VisitAccessibleModules([&](const core::ModuleDescriptor& module) {
    if (module.scope->ResolveDirect(symbol_name) == nullptr) {
      // continue search
      return true;
    }

    accept_candidate(module);

    // TODO: find ALL candidates?
    return false;
  });
}
}  // namespace

void UnresolvedSymbolLookup::ResolveDiagnostic(std::vector<lsp::CodeAction>& result,
                                               const lsp::CodeActionParams& params, const lsp::Diagnostic& diag,
                                               const core::SourceFile& file, LsSessionRef& d) {
  if (!diag.data || !diag.data->is_object() || !diag.data->contains(detail::codeAction::kPayloadKeyUnresolved)) {
    return;
  }

  const auto* n = ast::utils::GetNodeAt(file.ast, file.ast.lines.GetPosition(ast::Location{
                                                      .line = diag.range.start.line,
                                                      .column = diag.range.start.character + 1,
                                                  }));
  const auto symbol_name = n->On(file.ast.src);
  const auto loc = file.ast.lines.Translate(detail::FindPositionAfterLastImport(file.ast));
  const lsp::Range insertion_range{.start = conv::ToLSPPosition(loc), .end = conv::ToLSPPosition(loc)};
  actions::LookupMissingImport(*file.program, symbol_name, [&](const core::ModuleDescriptor& candidate) {
    result.emplace_back(lsp::CodeAction{
        .title = lib::FormatStringToArena(d.arena, "import module '{}' for symbol '{}'", candidate.name, symbol_name),
        .kind = lsp::CodeActionKind::kQuickfix,
        .isPreferred = true,
        .edit =
            lsp::WorkspaceEdit{
                .changes = {{{
                    params.textDocument.uri,
                    {lsp::TextEdit{
                        .range = insertion_range,
                        .newText = lib::FormatStringToArena(d.arena, "\nimport from {} all;", candidate.name),
                    }},
                }}},
            },
    });
  });
}

}  // namespace vanadium::ls::actions
