#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <print>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "detail/LanguageServerConv.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::CodeActionResult> methods::textDocument::codeAction::operator()(
    LsContext& ctx, const lsp::CodeActionParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  std::vector<lsp::CodeAction> actions;

  for (const auto& diag : params.context.diagnostics) {
    if (!diag.data) {
      std::println(stderr, " no data");
      continue;
    }
    const auto& data = *diag.data;
    std::println(stderr, " unresolved = {}, autofix = {}, dump={}", data.contains("unresolved"),
                 data.contains("autofix"), data.dump().value());
    if (data.contains("unresolved")) {
      const auto* n = core::ast::utils::GetNodeAt(file->ast, file->ast.lines.GetPosition(core::ast::Location{
                                                                 .line = diag.range.start.line,
                                                                 .column = diag.range.start.character + 1,
                                                             }));
      const auto text = n->On(file->ast.src);

      // TODO: multiple modules can provide resolution - consider continuing search
      project.program.VisitAccessibleModules([&](const core::ModuleDescriptor& module) {
        if (module.scope->ResolveDirect(text) == nullptr) {
          // continue search
          return true;
        }

        const auto& replacement = ctx->Temp<std::string>(std::format("import from {} all;\n", module.name));

        actions.emplace_back(
            lsp::CodeAction{
                .title = ctx->Temp<std::string>(std::format("Import module '{}' for symbol '{}'", module.name, text)),
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
                                                  file->ast.lines.Translate(data["autofix"]["range"][0].as<int>())),
                                              .end = conv::ToLSPPosition(file->ast.lines.Translate(
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
}  // namespace vanadium::ls
