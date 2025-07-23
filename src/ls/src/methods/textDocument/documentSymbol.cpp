#include <memory>
#include <optional>
#include <ranges>
#include <stack>
#include <vector>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"
#include "Program.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "detail/LanguageServerConv.h"

namespace vanadium::ls {

namespace {
void DumpParams(const core::SourceFile* file, const core::ast::nodes::FormalPars* params, std::string& buf) {
  if (params->list.empty()) {
    return;
  }

  buf += "(";

  const auto last_idx = std::ssize(params->list) - 1;
  for (const auto [idx, param] : params->list | std::views::enumerate) {
    if (param->restriction) {
      buf += file->Text(param->restriction);
      buf += " ";
    }

    buf += file->Text(param->type);

    if (param->name) {
      buf += " ";
      buf += file->Text(*param->name);
    }

    // TODO: arraydef

    if (idx != last_idx) {
      buf += ", ";
    }
  }

  buf += ")";
}
}  // namespace

template <>
rpc::ExpectedResult<lsp::DocumentSybmolResult> methods::textDocument::documentSymbol::operator()(
    LsContext& ctx, const lsp::DocumentSymbolParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  const auto lit = [&](const std::optional<core::ast::nodes::Ident>& ident) -> std::string_view {
    if (!ident) {
      return "";
    }
    return file->Text(*ident);
  };
  const auto nlit = [&](const core::ast::Node* n) -> std::string_view {
    return file->Text(n);
  };

  std::vector<lsp::DocumentSymbol> result;

  std::stack<std::vector<lsp::DocumentSymbol>*> hierarchy;
  hierarchy.emplace(&result);

  const auto push = [&](lsp::DocumentSymbol&& symbol) -> lsp::DocumentSymbol& {
    symbol.selectionRange = symbol.range;
    return hierarchy.top()->emplace_back(std::move(symbol));
  };

  const auto subtree = [&](lsp::DocumentSymbol&& symbol, std::invocable auto f) {
    auto& children_opt = push(std::move(symbol)).children;
    children_opt.emplace();

    hierarchy.emplace(std::addressof(*children_opt));
    f();
    hierarchy.pop();
  };

  bool inside_class{false};
  file->ast.root->Accept([&](this auto& self, const core::ast::Node* n) {
    switch (n->nkind) {
      case core::ast::NodeKind::Module: {
        const auto* m = n->As<core::ast::nodes::Module>();
        subtree(
            lsp::DocumentSymbol{
                .name = lit(m->name),
                .kind = lsp::SymbolKind::kPackage,
                .range = conv::ToLSPRange(m->nrange, file->ast),
            },
            [&] {
              m->Accept(self);
            });
        return false;
      }
      case core::ast::NodeKind::FuncDecl: {
        const auto* m = n->As<core::ast::nodes::FuncDecl>();

        auto& detail = ctx->Temp<std::string>();
        if (m->params) {
          detail.reserve(32 * m->params->list.size());
        }

        if (m->kind.kind != core::ast::TokenKind::FUNCTION) {
          detail += file->Text(m->kind.range);
        } else if (m->ret) {
          detail += nlit(m->ret);
          detail += " ";
        }
        if (m->params) {
          DumpParams(file, m->params, detail);
        }

        subtree(
            lsp::DocumentSymbol{
                .name = lit(m->name),
                .detail = detail,
                .kind = inside_class ? lsp::SymbolKind::kMethod : lsp::SymbolKind::kFunction,
                .range = conv::ToLSPRange(m->nrange, file->ast),
            },
            [&] {
              if (m->runs_on) {
                push(lsp::DocumentSymbol{
                    .name = "runs on",
                    .detail = nlit(m->runs_on->comp),
                    .kind = lsp::SymbolKind::kProperty,
                    .range = conv::ToLSPRange(m->runs_on->nrange, file->ast),
                });
              }
              if (m->system) {
                push(lsp::DocumentSymbol{
                    .name = "system",
                    .detail = nlit(m->system->comp),
                    .kind = lsp::SymbolKind::kProperty,
                    .range = conv::ToLSPRange(m->system->nrange, file->ast),
                });
              }
            });

        return false;
      }
      case core::ast::NodeKind::TemplateDecl: {
        const auto* m = n->As<core::ast::nodes::TemplateDecl>();

        auto& detail = ctx->Temp<std::string>();
        if (m->params) {
          detail.reserve(32 * m->params->list.size());
        }

        detail += nlit(m->type);
        detail += " ";
        if (m->params) {
          DumpParams(file, m->params, detail);
        }

        subtree(
            lsp::DocumentSymbol{
                .name = lit(m->name),
                .detail = detail,
                .kind = lsp::SymbolKind::kInterface,
                .range = conv::ToLSPRange(m->nrange, file->ast),
            },
            [&] {
              if (m->modif) {
                push(lsp::DocumentSymbol{
                    .name = "modifier",
                    .detail = file->Text(m->modif),
                    .kind = lsp::SymbolKind::kProperty,
                    .range = conv::ToLSPRange(m->modif->range, file->ast),
                });
              }
            });

        return false;
      }
      case core::ast::NodeKind::StructTypeDecl: {
        const auto* m = n->As<core::ast::nodes::StructTypeDecl>();

        subtree(
            lsp::DocumentSymbol{
                .name = lit(m->name),
                .detail = file->Text(m->kind.range),
                .kind = lsp::SymbolKind::kStruct,
                .range = conv::ToLSPRange(m->nrange, file->ast),
            },
            [&] {
              for (const auto* f : m->fields) {
                push(lsp::DocumentSymbol{
                    .name = lit(f->name),
                    .detail = nlit(f->type),
                    .kind = lsp::SymbolKind::kField,
                    .range = conv::ToLSPRange(f->nrange, file->ast),
                });
              }
            });

        return false;
      }
      case core::ast::NodeKind::EnumTypeDecl: {
        const auto* m = n->As<core::ast::nodes::EnumTypeDecl>();

        subtree(
            lsp::DocumentSymbol{
                .name = lit(m->name),
                .kind = lsp::SymbolKind::kEnum,
                .range = conv::ToLSPRange(m->nrange, file->ast),
            },
            [&] {
              for (const auto* e : m->enums) {
                push(lsp::DocumentSymbol{
                    .name = nlit(e),
                    .kind = lsp::SymbolKind::kEnummember,
                    .range = conv::ToLSPRange(e->nrange, file->ast),
                });
              }
            });

        return false;
      }
      case core::ast::NodeKind::SubTypeDecl: {
        const auto* m = n->As<core::ast::nodes::SubTypeDecl>();

        push(lsp::DocumentSymbol{
            .name = lit(m->field->name),
            .detail = "subtype",
            .kind = lsp::SymbolKind::kTypeparameter,
            .range = conv::ToLSPRange(m->nrange, file->ast),
        });

        return false;
      }
      case core::ast::NodeKind::ValueDecl: {
        const auto* m = n->As<core::ast::nodes::ValueDecl>();

        auto& detail = ctx->Temp<std::string>();
        if (m->kind) {
          detail += file->Text(m->kind->range);
          detail += " ";
        }
        detail += nlit(m->type);

        for (const auto* decl : m->decls) {
          push(lsp::DocumentSymbol{
              .name = lit(decl->name),
              .detail = detail,
              .kind = [&] -> lsp::SymbolKind {
                if (!m->kind) {
                  return lsp::SymbolKind::kEvent;
                }
                switch (m->kind->kind) {
                  case core::ast::TokenKind::PORT:
                    return lsp::SymbolKind::kInterface;
                  case core::ast::TokenKind::CONST:
                  case core::ast::TokenKind::TEMPLATE:
                  case core::ast::TokenKind::MODULEPAR:
                    return lsp::SymbolKind::kConstant;
                  default:
                    return lsp::SymbolKind::kVariable;
                }
              }(),
              .range = conv::ToLSPRange(decl->nrange, file->ast),
          });
        }

        return false;
      }
      case core::ast::NodeKind::ClassTypeDecl: {
        const auto* m = n->As<core::ast::nodes::ClassTypeDecl>();
        subtree(
            lsp::DocumentSymbol{
                .name = lit(m->name),
                .kind = lsp::SymbolKind::kClass,
                .range = conv::ToLSPRange(m->nrange, file->ast),
            },
            [&] {
              for (const auto* ext : m->extends) {
                push(lsp::DocumentSymbol{
                    .name = "extends",
                    .detail = nlit(ext),
                    .kind = lsp::SymbolKind::kProperty,
                    .range = conv::ToLSPRange(ext->nrange, file->ast),
                });
              }
              if (m->runs_on) {
                push(lsp::DocumentSymbol{
                    .name = "runs on",
                    .detail = nlit(m->runs_on->comp),
                    .kind = lsp::SymbolKind::kProperty,
                    .range = conv::ToLSPRange(m->runs_on->nrange, file->ast),
                });
              }
              if (m->system) {
                push(lsp::DocumentSymbol{
                    .name = "system",
                    .detail = nlit(m->system->comp),
                    .kind = lsp::SymbolKind::kProperty,
                    .range = conv::ToLSPRange(m->system->nrange, file->ast),
                });
              }
              inside_class = true;
              m->Accept(self);
              inside_class = false;
            });
        return false;
      }
      case core::ast::NodeKind::ConstructorDecl: {
        const auto* m = n->As<core::ast::nodes::ConstructorDecl>();

        std::string_view detail = "constructor";
        if (m->params) {
          auto& detail_buf = ctx->Temp<std::string>();
          detail_buf.reserve(32 * m->params->list.size());
          detail_buf += "constructor ";
          DumpParams(file, m->params, detail_buf);
          detail = detail_buf;
        }

        push(lsp::DocumentSymbol{
            .name = "create",
            .detail = detail,
            .kind = lsp::SymbolKind::kConstructor,
            .range = conv::ToLSPRange(m->nrange, file->ast),
        });

        return false;
      }
      case core::ast::NodeKind::BlockStmt: {
        return false;
      }
      default: {
        return true;
      }
    }
  });

  return result;
}
}  // namespace vanadium::ls
