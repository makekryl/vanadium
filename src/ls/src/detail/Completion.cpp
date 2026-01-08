#include "detail/Completion.h"

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/Metaprogramming.h>

#include <algorithm>
#include <glaze/json.hpp>
#include <string_view>
#include <variant>

#include "LSProtocol.h"
#include "LanguageServerConv.h"
#include "LanguageServerLogger.h"
#include "Program.h"
#include "Semantic.h"
#include "Solution.h"
#include "TypeChecker.h"
#include "detail/Definition.h"
#include "detail/Helpers.h"
#include "magic_enum/magic_enum.hpp"
#include "utils/SemanticUtils.h"

namespace vanadium::ls::detail {

// seems like it is unreliable to read string_view from generic,
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

namespace {
struct CompletionContext {
  const core::semantic::Scope* scope;
  std::vector<lsp::CompletionItem>& items;
  lib::Arena& arena;
};

// todo: extract to lib as strings concat
std::string_view Suborder(lib::Arena& arena, std::string_view s1, std::string_view s2) {
  auto buf = arena.AllocStringBuffer(s1.length() + s2.length());
  auto it = std::ranges::copy(s1, buf.begin()).out;
  it = std::ranges::copy(s2, it).out;
  return std::string_view(buf);
}
std::string_view Suborder(lib::Arena& arena, std::string_view s1, std::string_view s2, std::string_view s3) {
  auto buf = arena.AllocStringBuffer(s1.length() + s2.length() + s3.length());
  auto it = std::ranges::copy(s1, buf.begin()).out;
  it = std::ranges::copy(s2, it).out;
  it = std::ranges::copy(s3, it).out;
  return std::string_view(buf);
}

void CollectVisibleSymbols(CompletionContext& ctx, std::string_view order) {
  const auto bring_symbols = [&](const core::semantic::SymbolTable& symbols, std::string_view s_order,
                                 core::semantic::SymbolFlags::Value blacklisted_symbols =  // todo(!!!): reorganize
                                 core::semantic::SymbolFlags::kNone,
                                 std::optional<std::string_view> description = std::nullopt) {
    for (const auto& [name, sym] : symbols.Enumerate()) {
      if (sym.Flags() & blacklisted_symbols) {
        continue;
      }
      ctx.items.emplace_back(lsp::CompletionItem{
          .label = name,
          .labelDetails =
              lsp::CompletionItemLabelDetails{
                  // .detail = core::checker::utils::GetReadableTypeName(&sym), // <-- TODO: obtain type
                  .description = description,
              },
          .kind = LspSymbolKind(sym),
          .sortText = s_order,
      });
    }
  };

  const auto bring_from_augmentation = [&](const core::semantic::Scope* scope, std::string_view ks_order) {
    std::size_t i{0};
    for (const auto* t : scope->augmentation) {
      std::optional<std::string_view> description{std::nullopt};
      {
        auto r = t->Enumerate();
        const auto it = r.begin();
        if (it != r.end()) {
          const auto* decl = it->second.Declaration();
          if (decl->parent->nkind == ast::NodeKind::ValueDecl) {
            decl = decl->parent;
          }
          if (decl->parent->nkind == ast::NodeKind::Definition &&
              decl->parent->parent->nkind == ast::NodeKind::ComponentTypeDecl) {
            description =
                ast::utils::SourceFileOf(decl)->Text(*decl->parent->parent->As<ast::nodes::ComponentTypeDecl>()->name);
          }
        }
      }

      bring_symbols(*t, Suborder(ctx.arena, order, ks_order, std::to_string(i++)),
                    core::semantic::SymbolFlags::kImportedModule, description);
    }
  };

  bring_symbols(ctx.scope->symbols, Suborder(ctx.arena, order, "1"));
  bring_from_augmentation(ctx.scope, "1");

  {
    std::size_t i{0};
    for (const auto* cs = ctx.scope->ParentScope(); cs != nullptr; cs = cs->ParentScope()) {
      bring_symbols(cs->symbols, Suborder(ctx.arena, order, "2", std::to_string(i++)));
      bring_from_augmentation(cs, "2");
    }
  }
}
}  // namespace

lsp::CompletionList CollectCompletions(const lsp::CompletionParams& params, const core::SourceFile& file,
                                       LsSessionRef d) {
  if (!file.module) {
    return {};
  }

  constexpr std::size_t kMaxCompletionItems = 120;
  lsp::CompletionList completion_list{.isIncomplete = true};
  auto& items = completion_list.items;

  const auto* n = detail::FindNode(&file, params.position);

  VLS_WARN("--- compl:: n={}, parent={}, grandparent={}", magic_enum::enum_name(n->nkind),
           magic_enum::enum_name(n->parent->nkind), magic_enum::enum_name(n->parent->parent->nkind));

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file.module->scope, n);

  CompletionContext completion_ctx{
      .scope = scope,
      .items = items,
      .arena = d.arena,
  };

  const auto mask = file.Text(n);
  VLS_WARN("    compl:: mask: '{}'", mask);

  if (n->nkind == ast::NodeKind::Ident && n->parent->nkind == ast::NodeKind::SelectorExpr) {
    n = n->parent;
  }

  if (n->nkind == ast::NodeKind::SelectorExpr) {
    n = n->As<ast::nodes::SelectorExpr>()->x;
    const auto sym = core::checker::ResolveExprType(&file, scope, n->As<ast::nodes::Expr>());
    if (!sym) {
      return completion_list;  // todo: extract filler to separate func
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
    } else if (sym->Flags() & core::semantic::SymbolFlags::kList) {
      // // todo
      // items.emplace_back(lsp::CompletionItem{
      //     .label = "lengthof",
      //     .kind = lsp::CompletionItemKind::kSnippet,
      //     .textEdit =
      //         lsp::TextEdit{
      //             .range = conv::ToLSPRange(n->nrange, file.ast),
      //             .newText = *d.arena.Alloc<std::string>(std::format("lengthof({})", file.Text(n))),
      //         },
      // });
    } else if (sym->Flags() & core::semantic::SymbolFlags::kImportedModule) {
      // TODO REWRITE THIS ALL
      const auto* tgt_module = file.program->GetModule(sym->GetName());
      if (tgt_module) {
        for (const auto& [name, msym] : tgt_module->scope->symbols.Enumerate()) {
          // TODO: unify, reduce code
          if (msym.Flags() & core::semantic::SymbolFlags::kImportedModule) {
            continue;
          }
          // TODO: show type
          items.emplace_back(lsp::CompletionItem{
              .label = name,
              .kind = LspSymbolKind(msym),
              .sortText = "0",
              .insertText = name,
          });
        }
      }
    }
    return completion_list;  // todo: extract filler to separate func
  }

  const auto complete_props = [&](const core::semantic::Symbol* sym) {
    if (!sym) {
      return;
    }
    if (sym->Flags() & core::semantic::SymbolFlags::kStructural) {
      for (const auto& [name, msym] : sym->Members()->Enumerate()) {
        // TODO: show type
        items.emplace_back(lsp::CompletionItem{
            .label = name,
            .kind = lsp::CompletionItemKind::kProperty,
            .sortText = "0",
            .insertText = *d.arena.Alloc<std::string>(std::format("{} := ", name)),
        });
      }
    } else if (sym->Flags() & core::semantic::SymbolFlags::kList) {
      // TODO
    }
  };

  const auto* parent = n->parent;
  switch (parent->nkind) {
    case ast::NodeKind::Definition: {
      return completion_list;  // todo: extract filler to separate func
    }
    case ast::NodeKind::ImportDecl: {
      file.program->VisitAccessibleModules([&](const core::ModuleDescriptor& mod) {
        if (mod.name.contains(mask) && !file.module->imports.contains(mod.name)) {
          items.emplace_back(lsp::CompletionItem{
              .label = mod.name,
              .kind = lsp::CompletionItemKind::kModule,
              .detail = mod.sf->path,
              .sortText = "7",
          });
        }
        return true;
      });
      break;
    }
    case ast::NodeKind::CompositeLiteral:
      complete_props(
          core::checker::ext::DeduceCompositeLiteralType(&file, scope, parent->As<ast::nodes::CompositeLiteral>()).sym);
      break;
    case ast::NodeKind::ParenExpr: {
      const auto* pe = parent->As<ast::nodes::ParenExpr>();
      const auto* ce = pe->parent->As<ast::nodes::CallExpr>();
      const auto sym = core::checker::ResolveExprType(&file, scope, ce->fun);
      if (sym && (sym->Flags() & (core::semantic::SymbolFlags::kFunction | core::semantic::SymbolFlags::kTemplate))) {
        const auto* params = ast::utils::GetCallableDeclParams(sym->Declaration()->As<ast::nodes::Decl>());
        const auto* params_file = ast::utils::SourceFileOf(params);
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
    case ast::NodeKind::AssignmentExpr: {
      const auto* ae = parent->As<ast::nodes::AssignmentExpr>();
      if ((n == ae->property) && (ae->parent->nkind == ast::NodeKind::CompositeLiteral/* ||
                                    ae->parent->nkind == ast::NodeKind::ParenExpr*/)) {
        complete_props(
            core::checker::ext::DeduceCompositeLiteralType(&file, scope, ae->parent->As<ast::nodes::CompositeLiteral>())
                .sym);
        return completion_list;  // todo: extract filler to separate func
      }

      break;
    }

    default:
      break;
  }

  const auto expected_type_opt = core::checker::ext::DeduceExpectedType(&file, scope, n);
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
    return completion_list;  // todo: extract filler to separate func
  }

  CollectVisibleSymbols(completion_ctx, "1");

  file.program->VisitAccessibleModules([&](const core::ModuleDescriptor& module) {
    for (const auto& sym : module.scope->symbols.Enumerate() | std::views::values) {
      if (sym.Flags() & core::semantic::SymbolFlags::kImportedModule) {
        continue;
      }
      if ((sym.Flags() & core::semantic::SymbolFlags::kFunction) == core::semantic::SymbolFlags::kFunction) {
        const auto* funcdecl = sym.Declaration()->As<ast::nodes::FuncDecl>();
        if (funcdecl->kind.kind == ast::TokenKind::TESTCASE) {
          continue;
        }
      }
      if (!sym.GetName().contains(mask)) {
        continue;
      }

      const core::semantic::Symbol* actual_type =
          (sym.Flags() & (core::semantic::SymbolFlags::kFunction | core::semantic::SymbolFlags::kTemplate))
              ? core::checker::ResolveCallableReturnType(module.sf, sym.Declaration()->As<ast::nodes::Decl>()).sym
              : core::checker::ResolveDeclarationType(module.sf, sym.Declaration()->As<ast::nodes::Decl>()).sym;
      if (expected_type_opt && expected_type_opt.sym != actual_type) {
        continue;
      }
      items.emplace_back(lsp::CompletionItem{
          .label = sym.GetName(),
          .kind = LspSymbolKind(sym),
          .detail = module.name,
          .sortText = "4",
          // TODO: find a way to have strongly-typed 'data' on both ends (maybe modify lspgen to produce templates,
          // exposing internal payload structure to the higher levels does not sounds good though - and it will be
          // required to do in such case)
          .data = glz::generic{{"path", file.path},
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

  return completion_list;  // todo: extract filler to separate func
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
            const ast::pos_t last_import_pos = FindPositionAfterLastImport(file->ast);
            if (last_import_pos == ast::pos_t(-1)) {
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
