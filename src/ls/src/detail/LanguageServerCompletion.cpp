#include "detail/LanguageServerCompletion.h"

#include <print>

#include "AST.h"
#include "ASTNodes.h"
#include "ASTTypes.h"
#include "Arena.h"
#include "LSProtocol.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "utils/ASTUtils.h"

namespace vanadium::ls::detail {

void PerformCompletion(const core::SourceFile* file, const core::ast::Node* n, lib::Arena& arena,
                       std::vector<lsp::CompletionItem>& items, std::size_t limit) {
  if (n->nkind != core::ast::NodeKind::Ident) {
    return;
  }
  const auto mask = file->Text(n);
  std::println(stderr, "mask: '{}'", mask);

  const auto* parent = n->parent;
  switch (parent->nkind) {
    case core::ast::NodeKind::ImportDecl: {
      file->program->VisitAccessibleModules([&](const core::ModuleDescriptor& module) {
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

  if (core::ast::utils::GetPredecessor<core::ast::nodes::BlockStmt>(parent)) {
    file->program->VisitAccessibleModules([&](const core::ModuleDescriptor& module) {
      for (const auto& sym : module.scope->symbols.Enumerate() | std::views::values) {
        if (sym.Flags() & core::semantic::SymbolFlags::kImportedModule) {
          continue;
        }
        if ((sym.Flags() & core::semantic::SymbolFlags::kFunction) == core::semantic::SymbolFlags::kFunction) {
          const auto* funcdecl = sym.Declaration()->As<core::ast::nodes::FuncDecl>();
          if (funcdecl->kind.kind == core::ast::TokenKind::TESTCASE) {
            continue;
          }
        }
        if (!sym.GetName().contains(mask)) {
          continue;
        }
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
                case core::semantic::SymbolFlags::kEnumMember:
                  return lsp::CompletionItemKind::kEnummember;
                case core::semantic::SymbolFlags::kClass:
                  return lsp::CompletionItemKind::kClass;
                default:
                  return lsp::CompletionItemKind::kUnit;
              }
            }(),
            .detail = module.name,
            .sortText = "0",
            .additionalTextEdits = {{
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
                    .newText = *arena.Alloc<std::string>(std::format("import from {} all;\n", module.name)),
                },
            }},
            .commitCharacters = {{"("}},
        });
        if (items.size() >= limit) {
          return false;
        }
      }
      return true;
    });
  }
}

}  // namespace vanadium::ls::detail
