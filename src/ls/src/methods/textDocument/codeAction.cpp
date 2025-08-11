#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerLogger.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "Program.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"

namespace vanadium::ls {
namespace {
lsp::CodeActionResult ProvideCodeActions(const lsp::CodeActionParams& params, const core::SourceFile& file,
                                         LsSessionRef d) {
  std::vector<lsp::CodeAction> actions;

  for (const auto& diag : params.context.diagnostics) {
    if (!diag.data) {
      VLS_DEBUG(" no data");
      continue;
    }
    const auto& data = *diag.data;
    VLS_DEBUG("CA data='{}'", data.dump().value());
    if (data.contains("unresolved")) {
      const auto* n = core::ast::utils::GetNodeAt(file.ast, file.ast.lines.GetPosition(core::ast::Location{
                                                                .line = diag.range.start.line,
                                                                .column = diag.range.start.character + 1,
                                                            }));
      const auto text = n->On(file.ast.src);

      // TODO: multiple modules can provide resolution - consider continuing search
      file.program->VisitAccessibleModules([&](const core::ModuleDescriptor& module) {
        if (module.scope->ResolveDirect(text) == nullptr) {
          // continue search
          return true;
        }

        const auto& replacement = *d.arena.Alloc<std::string>(std::format("import from {} all;\n", module.name));

        actions.emplace_back(
            lsp::CodeAction{
                .title =
                    *d.arena.Alloc<std::string>(std::format("Import module '{}' for symbol '{}'", module.name, text)),
                .kind = lsp::CodeActionKind::kQuickfix,
                .isPreferred = true,
                .edit =
                    lsp::WorkspaceEdit{
                        .changes =
                            std::unordered_map<std::string_view, std::vector<lsp::TextEdit>>{
                                {
                                    params.textDocument.uri,
                                    {lsp::TextEdit{
                                        .range =
                                            lsp::Range{
                                                .start =
                                                    lsp::Position{
                                                        .line = 2,
                                                        .character = 0,
                                                    },
                                                .end =
                                                    lsp::Position{
                                                        .line = 2,
                                                        .character = 0,
                                                    },
                                            },
                                        .newText = replacement,
                                    }},
                                }},
                    },
            });

        return false;
      });
    } else if (data.contains("autofix")) {
      actions.emplace_back(
          lsp::CodeAction{
              .title = data["autofix"]["title"].as<std::string_view>(),
              .kind = lsp::CodeActionKind::kQuickfix,
              .isPreferred = true,
              .edit =
                  lsp::WorkspaceEdit{
                      .changes =
                          std::unordered_map<std::string_view, std::vector<lsp::TextEdit>>{
                              {
                                  params.textDocument.uri,
                                  {lsp::TextEdit{
                                      .range =
                                          lsp::Range{
                                              .start = conv::ToLSPPosition(
                                                  file.ast.lines.Translate(data["autofix"]["range"][0].as<int>())),
                                              .end = conv::ToLSPPosition(file.ast.lines.Translate(
                                                  data["autofix"]["range"][1].as<int>())),
                                          },
                                      .newText = "",
                                  }},
                              }},
                  },
          });
    }
  }

  return actions;
}
}  // namespace

template <>
rpc::ExpectedResult<lsp::CodeActionResult> methods::textDocument::codeAction::operator()(
    LsContext& ctx, const lsp::CodeActionParams& params) {
  return ctx->WithFile<lsp::CodeActionResult>(params, ProvideCodeActions).value_or(nullptr);
}
}  // namespace vanadium::ls
