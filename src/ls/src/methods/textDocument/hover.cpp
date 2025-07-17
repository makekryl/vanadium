#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <ranges>

#include "AST.h"
#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {

namespace {
template <core::ast::IsNode TParamDescriptorNode>
std::string BuildMarkdownParameterList(const core::ast::AST& ast, std::span<const TParamDescriptorNode* const> params) {
  std::string buf;
  buf.reserve(params.size() * 32);

  const auto last_idx = static_cast<decltype(params)::difference_type>(params.size()) - 1;
  for (const auto& [idx, param] : params | std::views::enumerate) {
    const core::ast::Node* typenode = param->type;
    if (typenode->nkind == core::ast::NodeKind::SelectorExpr) {
      typenode = core::ast::utils::TraverseSelectorExpressionStart(typenode->As<core::ast::nodes::SelectorExpr>());
    }

    buf += "- `";

    buf += ast.Text(typenode);
    buf += " ";
    buf += ast.Text(*param->name);

    if constexpr (std::is_same_v<TParamDescriptorNode, core::ast::nodes::Field>) {
      if (param->optional) {
        buf += " (optional)";
      }
    }

    buf += "`";

    if (idx != last_idx) [[likely]] {
      buf += "\n";
    }
  }

  return buf;
}
}  // namespace

template <>
rpc::ExpectedResult<lsp::HoverResult> methods::textDocument::hover::operator()(LsContext& ctx,
                                                                               const lsp::HoverParams& params) {
  const auto& [subproject, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = subproject.program.GetFile(path);

  const auto* n = core::ast::utils::GetNodeAt(file->ast, file->ast.lines.GetPosition(core::ast::Location{
                                                             .line = params.position.line,
                                                             .column = params.position.character,
                                                         }));

  if (n->nkind != core::ast::NodeKind::Ident) {
    return nullptr;
  }

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, n);
  const auto* sym = scope->Resolve(n->On(file->ast.src));
  if (!sym) {
    return nullptr;
  }

  auto& content = *ctx->GetTemporaryArena().Alloc<std::string>();
  content.reserve(256);  // TODO: check if it is justified actually

  const auto* decl = sym->Declaration();
  const auto* provider_file = core::ast::utils::SourceFileOf(decl);

  switch (decl->nkind) {
    case core::ast::NodeKind::FuncDecl: {
      const auto* m = decl->As<core::ast::nodes::FuncDecl>();
      content += std::format(
          R"(
  ### {} `{}`
---
→ `{}`
{}
---
```ttcn
{}
```
)",
          provider_file->Text(m->kind.range),  //
          provider_file->Text(*m->name),       //
          m->ret ? provider_file->ast.Text(m->ret->type) : "void",
          m->params->list.empty()
              ? ""
              : std::format("\nArguments:\n{}", BuildMarkdownParameterList<core::ast::nodes::FormalPar>(
                                                    provider_file->ast, m->params->list)),
          provider_file->ast.Text(core::ast::Range{
              .begin = decl->nrange.begin,
              .end = m->body->nrange.begin,
          }));
      break;
    }
    case core::ast::NodeKind::TemplateDecl: {
      const auto* m = decl->As<core::ast::nodes::TemplateDecl>();
      content +=
          std::format(R"(
### template `{}`
---
Parameters:
{}
---
```ttcn
{}
```
)",
                      provider_file->ast.Text(*m->name),  //
                      BuildMarkdownParameterList<core::ast::nodes::FormalPar>(provider_file->ast, m->params->list),
                      provider_file->ast.Text(core::ast::Range{
                          .begin = decl->nrange.begin,
                          .end = m->value->nrange.begin,
                      }));
      break;
    }
    case core::ast::NodeKind::StructTypeDecl: {
      const auto* m = decl->As<core::ast::nodes::StructTypeDecl>();
      content += std::format(R"(
### {} `{}`
---
Fields:
{}
---
```ttcn
{}
```
)",
                             provider_file->Text(m->kind.range),  //
                             provider_file->ast.Text(*m->name),
                             BuildMarkdownParameterList<core::ast::nodes::Field>(provider_file->ast, m->fields),
                             provider_file->ast.Text(core::ast::Range{
                                 .begin = m->nrange.begin,
                                 .end = m->name->nrange.end,
                             }));
      break;
    }
    case core::ast::NodeKind::EnumTypeDecl: {
      const auto* m = decl->As<core::ast::nodes::EnumTypeDecl>();
      content += std::format(
          R"(
### enumerated `{}`
---
Values:
{}
---
```ttcn
{}
```
)",
          provider_file->ast.Text(*m->name),
          [&] -> std::string {
            std::string buf;
            buf.reserve(m->enums.size() * 32);

            const auto last_idx = static_cast<decltype(m->enums)::difference_type>(m->enums.size()) - 1;
            for (const auto& [idx, v] : m->enums | std::views::enumerate) {
              buf += "- `";
              buf += provider_file->Text(v);
              buf += "`";
              if (idx != last_idx) [[likely]] {
                buf += "\n";
              }
            }

            return buf;
          }(),
          provider_file->ast.Text(core::ast::Range{
              .begin = m->nrange.begin,
              .end = m->name->nrange.end,
          }));
      break;
    }
    case core::ast::NodeKind::SubTypeDecl: {
      const auto* m = decl->As<core::ast::nodes::SubTypeDecl>();
      content += std::format(R"(
### subtype `{}`
---
```ttcn
{}
```
)",
                             provider_file->ast.Text(*m->field->name),
                             provider_file->ast.Text(core::ast::Range{
                                 .begin = m->nrange.begin,
                                 .end = m->field->name->nrange.end,
                             }));
      break;
    }
    default:
      content += "TODO";
      break;
  }

  if (provider_file != file) {
    const auto source_loc = conv::ToLSPPosition(provider_file->ast.lines.Translate(decl->nrange.begin));
    content += std::format(R"(
---
[module {}]({}#L{}C{}))",
                           provider_file->module->name, ctx->PathToFileUri(provider_file->path), source_loc.line,
                           source_loc.character);
  }
  return lsp::Hover{
      .contents =
          lsp::MarkupContent{
              .kind = lsp::MarkupKind::kMarkdown,
              .value = content,
          },
      .range = conv::ToLSPRange(n->nrange, file->ast),
  };
}
}  // namespace vanadium::ls
