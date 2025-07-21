#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <print>
#include <ranges>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "ImportVisitor.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Program.h"
#include "Semantic.h"
#include "domain/LanguageServerSymbolDef.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

static constexpr std::size_t kMaxCompletionItems = 64;

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::CompletionResult> methods::textDocument::completion::operator()(
    LsContext& ctx, const lsp::CompletionParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  std::vector<lsp::CompletionItem> items;

  const auto* tgt = domain::FindNode(file, params.position);
  if (tgt->nkind != core::ast::NodeKind::Ident) {
    return nullptr;
  }
  const auto mask = file->Text(tgt);
  std::println(stderr, "FUCK ELON MUSK: '{}'", mask);

  const auto* n = tgt->parent;
  switch (n->nkind) {
    case core::ast::NodeKind::ImportDecl: {
      project.program.VisitAccessibleModules([&](const core::ModuleDescriptor& module) {
        if (module.name.contains(mask)) {
          items.emplace_back(lsp::CompletionItem{
              .label = module.name,
              .kind = lsp::CompletionItemKind::kModule,
              .detail = module.sf->path,
              .sortText = "0",
          });
        }
        return true;
      });
      break;
    }
    default:
      break;
  }

  if (core::ast::utils::GetPredecessor<core::ast::nodes::BlockStmt>(n)) {
    // const auto* scope = core::semantic::utils::FindScope(file->module->scope, n);
    project.program.VisitAccessibleModules([&](const core::ModuleDescriptor& module) {
      for (const auto& sym : module.scope->symbols.Enumerate() | std::views::values) {
        items.emplace_back(lsp::CompletionItem{
            .label = sym.GetName(),
            .kind = [&] -> lsp::CompletionItemKind {
              switch (sym.Flags()) {
                case core::semantic::SymbolFlags::kFunction:
                  return lsp::CompletionItemKind::kFunction;
                case core::semantic::SymbolFlags::kStructural:
                  return lsp::CompletionItemKind::kStruct;
                case core::semantic::SymbolFlags::kEnum:
                  return lsp::CompletionItemKind::kEnum;
                case core::semantic::SymbolFlags::kEnumValue:
                  return lsp::CompletionItemKind::kEnummember;
                case core::semantic::SymbolFlags::kClass:
                  return lsp::CompletionItemKind::kClass;
                default:
                  return lsp::CompletionItemKind::kUnit;
              }
            }(),
            .detail = module.name,
            .sortText = "0",
            .additionalTextEdits =
                std::vector<lsp::TextEdit>{
                    lsp::TextEdit{
                        .range =
                            lsp::Range{
                                .start =
                                    lsp::Position{
                                        .line = 1,
                                        .character = 0,
                                    },
                                .end =
                                    lsp::Position{
                                        .line = 1,
                                        .character = 0,
                                    },
                            },
                        .newText = ctx->Temp<std::string>(std::format("import from {} all;\n", module.name)),
                    },
                },
        });
        if (items.size() >= kMaxCompletionItems) {
          return false;
        }
      }
      return true;
    });
  }

  return items;
}
}  // namespace vanadium::ls
