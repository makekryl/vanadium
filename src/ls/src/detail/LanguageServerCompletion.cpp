#include "detail/LanguageServerCompletion.h"

#include <algorithm>
#include <glaze/core/read.hpp>
#include <glaze/json/write.hpp>
#include <print>
#include <variant>

#include "AST.h"
#include "ASTNodes.h"
#include "ASTTypes.h"
#include "Arena.h"
#include "LSProtocol.h"
#include "LanguageServerConv.h"
#include "Metaprogramming.h"
#include "Program.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "detail/LanguageServerCoreUtils.h"
#include "utils/ASTUtils.h"

namespace vanadium::ls::detail {

// seems like it is unreliable to read string_view from json_t,
// so we have to stick with extra allocations
struct CompletionItemPayload {
  struct ExternalDefinition {
    std::string module;
  };

  std::string path;
  std::variant<ExternalDefinition> details;
};

void CollectCompletions(const core::SourceFile* file, const core::ast::Node* n, lib::Arena& arena,
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
            .commitCharacters = {{"("}},
            // TODO: find a way to have strongly-typed 'data' on both ends (maybe modify lspgen to produce templates,
            // exposing internal payload structure to the higher levels does not sounds good though - and it will be
            // required to do in such case)
            .data = glz::json_t{{"path", file->path},
                                {"details",
                                 {
                                     {"module", module.name},
                                 }}},
        });
        if (items.size() >= limit) {
          return false;
        }
      }
      return true;
    });
  }
}  // namespace vanadium::ls::detail

std::optional<lsp::CompletionItem> ResolveCompletionItem(const tooling::Solution& solution, lib::Arena& arena,
                                                         const lsp::CompletionItem& original_completion) {
  if (!original_completion.data) {
    return std::nullopt;
  }
  const auto payload = glz::read_json<CompletionItemPayload>(*original_completion.data);

  const auto* project = solution.ProjectOf((solution.RootDirectory() / payload->path).string());
  const auto* file = project->program.GetFile(payload->path);

  lsp::CompletionItem rendition{original_completion};

  std::visit(
      mp::Overloaded{
          [&](const CompletionItemPayload::ExternalDefinition& def) {
            if (file->module->imports.contains(def.module) &&
                std::ranges::any_of(file->module->ImportsOf(def.module), [](const core::ImportDescriptor& import) {
                  return !import.transit;
                })) {
              // module is already imported
              return;
            }
            const core::ast::pos_t last_import_pos = FindPositionAfterLastImport(file->ast);
            if (last_import_pos == core::ast::pos_t(-1)) {
              return;
            }
            const auto loc = file->ast.lines.Translate(last_import_pos);
            rendition.additionalTextEdits = {{
                lsp::TextEdit{
                    .range =
                        lsp::Range{
                            .start = conv::ToLSPPosition(loc),
                            .end = conv::ToLSPPosition(loc),
                        },
                    .newText = *arena.Alloc<std::string>(std::format("\nimport from {} all;\n", def.module)),
                },
            }};
          },
      },
      payload->details);

  return rendition;
}

}  // namespace vanadium::ls::detail
