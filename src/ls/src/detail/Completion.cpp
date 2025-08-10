#include "detail/Completion.h"

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
#include "LanguageServerLogger.h"
#include "Metaprogramming.h"
#include "Program.h"
#include "Semantic.h"
#include "Solution.h"
#include "TypeChecker.h"
#include "detail/Definition.h"
#include "detail/Helpers.h"
#include "magic_enum/magic_enum.hpp"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

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

namespace {
[[nodiscard]] lsp::CompletionItemKind LspSymbolKind(const core::semantic::Symbol& sym) {
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
    case core::semantic::SymbolFlags::kVariable:
      return lsp::CompletionItemKind::kVariable;
    default:
      return lsp::CompletionItemKind::kUnit;
  }
}
}  // namespace

std::vector<lsp::CompletionItem> CollectCompletions(const lsp::CompletionParams& params, const core::SourceFile& file,
                                                    LsSessionRef d) {
  if (!file.module) {
    return {};
  }

  constexpr std::size_t kMaxCompletionItems = 120;
  std::vector<lsp::CompletionItem> items;

  const auto* n = detail::FindNode(&file, params.position);

  VLS_DEBUG("n={}, parent={}, grandparent={}", magic_enum::enum_name(n->nkind), magic_enum::enum_name(n->parent->nkind),
            magic_enum::enum_name(n->parent->parent->nkind));

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file.module->scope, n);

  const auto mask = file.Text(n);
  VLS_DEBUG("mask: '{}'", mask);

  if (n->nkind == core::ast::NodeKind::SelectorExpr) {
    n = n->As<core::ast::nodes::SelectorExpr>()->x;
    const auto* sym = core::checker::ResolveExprType(&file, scope, n->As<core::ast::nodes::Expr>());
    if (!sym) {
      return items;  // todo: extract filler to separate func
    }
    if (sym->Flags() & core::semantic::SymbolFlags::kStructural) {
      for (const auto& [name, msym] : sym->Members()->Enumerate()) {
        // TODO: show type
        items.emplace_back(lsp::CompletionItem{
            .label = name,
            .kind = lsp::CompletionItemKind::kProperty,
            .sortText = "0",
        });
      }
    } else if (sym->Flags() & core::semantic::SymbolFlags::kClass) {
      for (const auto& [name, msym] : sym->OriginatedScope()->symbols.Enumerate()) {
        // TODO: show type
        items.emplace_back(lsp::CompletionItem{
            .label = name,
            .kind = LspSymbolKind(msym),
            .sortText = "0",
        });
      }
    }
    return items;  // todo: extract filler to separate func
  }

  const auto complete_props = [&](const core::semantic::Symbol* sym) {
    if (!sym || !(sym->Flags() & core::semantic::SymbolFlags::kStructural)) {
      return;
    }
    for (const auto& [name, msym] : sym->Members()->Enumerate()) {
      // TODO: show type
      items.emplace_back(lsp::CompletionItem{
          .label = name,
          .kind = lsp::CompletionItemKind::kProperty,
          .sortText = "0",
          .insertText = *d.arena.Alloc<std::string>(std::format("{} := ", name)),
      });
    }
  };

  const auto* parent = n->parent;
  switch (parent->nkind) {
    case core::ast::NodeKind::Definition: {
      return items;  // todo: extract filler to separate func
    }
    case core::ast::NodeKind::ImportDecl: {
      file.program->VisitAccessibleModules([&](const core::ModuleDescriptor& mod) {
        if (mod.name.contains(mask) && !file.module->imports.contains(mod.name)) {
          items.emplace_back(lsp::CompletionItem{
              .label = mod.name,
              .kind = lsp::CompletionItemKind::kModule,
              .detail = mod.sf->path,
              .sortText = "0",
          });
        }
        return true;
      });
      break;
    }
    case core::ast::NodeKind::CompositeLiteral:
      complete_props(core::checker::ext::DeduceCompositeLiteralType(&file, scope,
                                                                    parent->As<core::ast::nodes::CompositeLiteral>()));
      break;
    case core::ast::NodeKind::ParenExpr: {
      const auto* pe = parent->As<core::ast::nodes::ParenExpr>();
      const auto* ce = pe->parent->As<core::ast::nodes::CallExpr>();
      const auto* sym = core::checker::ResolveExprType(&file, scope, ce->fun);
      if (sym && (sym->Flags() & (core::semantic::SymbolFlags::kFunction | core::semantic::SymbolFlags::kTemplate))) {
        const auto* params = core::ast::utils::GetCallableDeclParams(sym->Declaration()->As<core::ast::nodes::Decl>());
        const auto* params_file = core::ast::utils::SourceFileOf(params);
        for (const auto* param : params->list) {
          // TODO: detail, doc
          const auto name = params_file->Text(*param->name);
          items.emplace_back(lsp::CompletionItem{
              .label = name,
              .kind = lsp::CompletionItemKind::kProperty,
              .sortText = "1",
              .insertText = *d.arena.Alloc<std::string>(std::format("{} := ", name)),
          });
        }
      }
      break;
    }
    case core::ast::NodeKind::AssignmentExpr: {
      if (parent->nkind == core::ast::NodeKind::AssignmentExpr) {
        const auto* ae = parent->As<core::ast::nodes::AssignmentExpr>();
        if ((n == ae->property) && (ae->parent->nkind == core::ast::NodeKind::CompositeLiteral/* ||
                                    ae->parent->nkind == core::ast::NodeKind::ParenExpr*/)) {
          complete_props(core::checker::ext::DeduceCompositeLiteralType(
              &file, scope, ae->parent->As<core::ast::nodes::CompositeLiteral>()));
          return items;  // todo: extract filler to separate func
        }
      }

      break;
    }

    default:
      break;
  }

  const core::semantic::Symbol* expected_type_opt = core::checker::ext::DeduceExpectedType(&file, scope, n);
  if (expected_type_opt && (expected_type_opt->Flags() & core::semantic::SymbolFlags::kEnum)) {
    for (const auto& [name, msym] : expected_type_opt->Members()->Enumerate()) {
      items.emplace_back(lsp::CompletionItem{
          .label = name,
          .kind = lsp::CompletionItemKind::kEnummember,
          .detail = expected_type_opt->GetName(),
          .sortText = "0",
      });
    }
    // nothing more needed
    return items;  // todo: extract filler to separate func
  }

  for (const auto* cs = scope; cs != nullptr; cs = cs->ParentScope()) {
    for (const auto& [name, msym] : cs->symbols.Enumerate()) {
      items.emplace_back(lsp::CompletionItem{
          .label = name,
          .kind = LspSymbolKind(msym),
          .sortText = "0",
      });
    }
  }

  file.program->VisitAccessibleModules([&](const core::ModuleDescriptor& module) {
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
      if (expected_type_opt &&
          expected_type_opt != core::checker::ResolveCallableReturnType(
                                   module.sf, module.scope, sym.Declaration()->As<core::ast::nodes::Decl>())) {
        continue;
      }
      items.emplace_back(lsp::CompletionItem{
          .label = sym.GetName(),
          .kind = LspSymbolKind(sym),
          .detail = module.name,
          .sortText = "2",
          // TODO: find a way to have strongly-typed 'data' on both ends (maybe modify lspgen to produce templates,
          // exposing internal payload structure to the higher levels does not sounds good though - and it will be
          // required to do in such case)
          .data = glz::json_t{{"path", file.path},
                              {"details",
                               {
                                   {"module", module.name},
                               }}},
      });
      if (items.size() >= kMaxCompletionItems) {
        return false;
      }
    }
    return true;
  });

  return items;  // todo: extract filler to separate func
}

std::optional<lsp::CompletionItem> ResolveCompletionItem(const lsp::CompletionItem& original_completion,
                                                         LsSessionRef d) {
  if (!original_completion.data) {
    return std::nullopt;
  }
  const auto payload = glz::read_json<CompletionItemPayload>(*original_completion.data);

  const auto* project =
      d.solution.ProjectOf((std::filesystem::path(d.solution.Directory().base_path) / payload->path).string());
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
                    .newText = *d.arena.Alloc<std::string>(std::format("\nimport from {} all;\n", def.module)),
                },
            }};
          },
      },
      payload->details);

  return rendition;
}

}  // namespace vanadium::ls::detail
