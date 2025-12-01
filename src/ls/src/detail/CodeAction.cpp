#include "detail/CodeAction.h"

#include "AST.h"
#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LanguageServerConv.h"
#include "LanguageServerLogger.h"
#include "Program.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "detail/Helpers.h"
#include "utils/ASTUtils.h"

namespace vanadium::ls::detail {

lsp::CodeActionResult ProvideCodeActions(const lsp::CodeActionParams& params, const core::SourceFile& file,
                                         LsSessionRef d) {
  std::vector<lsp::CodeAction> actions;

  for (const auto& diag : params.context.diagnostics) {
    if (!diag.data) {
      continue;
    }
    const auto& data = *diag.data;
    VLS_DEBUG("CA data='{}'", data.dump().value());
    if (data.contains(codeAction::kPayloadKeyUnresolved)) {  // TODO: extract to shared constant
      const auto* n = ast::utils::GetNodeAt(file.ast, file.ast.lines.GetPosition(ast::Location{
                                                          .line = diag.range.start.line,
                                                          .column = diag.range.start.character + 1,
                                                      }));
      const auto text = n->On(file.ast.src);

      file.program->VisitAccessibleModules([&](const core::ModuleDescriptor& module) {
        if (module.scope->ResolveDirect(text) == nullptr) {
          // continue search
          return true;
        }

        const auto& replacement = *d.arena.Alloc<std::string>(std::format("\nimport from {} all;", module.name));

        const ast::pos_t last_import_pos = detail::FindPositionAfterLastImport(file.ast);
        if (last_import_pos == ast::pos_t(-1)) {
          return false;
        }
        const auto loc = file.ast.lines.Translate(last_import_pos);

        actions.emplace_back(lsp::CodeAction{
            .title = *d.arena.Alloc<std::string>(std::format("import module '{}' for symbol '{}'", module.name, text)),
            .kind = lsp::CodeActionKind::kQuickfix,
            .isPreferred = true,
            .edit =
                lsp::WorkspaceEdit{
                    .changes = {{{
                        params.textDocument.uri,
                        {lsp::TextEdit{
                            .range = lsp::Range{.start = conv::ToLSPPosition(loc), .end = conv::ToLSPPosition(loc)},
                            .newText = replacement,
                        }},
                    }}},
                },
        });

        return false;
      });
    } else if (data.contains(codeAction::kPayloadKeyAutofix)) {  // TODO: extract to shared constant
      const auto& af_data = data[codeAction::kPayloadKeyAutofix];
      actions.emplace_back(
          lsp::CodeAction{
              .title = af_data["title"].as<std::string_view>(),
              .kind = lsp::CodeActionKind::kQuickfix,
              .isPreferred = true,
              .edit =
                  lsp::WorkspaceEdit{
                      .changes = {{{params.textDocument.uri,
                                    {lsp::TextEdit{
                                        .range = lsp::Range{.start = conv::ToLSPPosition(file.ast.lines.Translate(
                                                                af_data["range"][0].as<int>())),
                                                            .end = conv::ToLSPPosition(file.ast.lines.Translate(
                                                                af_data["range"][1].as<int>()))},
                                        .newText = af_data["replacement"].as<std::string_view>(),
                                    }}}}},
                  },
          });
    }
  }

  return actions;
}

}  // namespace vanadium::ls::detail
