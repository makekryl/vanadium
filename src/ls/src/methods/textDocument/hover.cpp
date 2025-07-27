#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <iterator>
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
#include "detail/Definition.h"
#include "utils/ASTUtils.h"

namespace vanadium::ls {

namespace {
template <core::ast::IsNode TParamDescriptorNode>
std::string BuildMarkdownParameterList(const core::ast::AST& ast, std::span<const TParamDescriptorNode* const> params) {
  std::string buf;
  buf.reserve(params.size() * 32);

  const auto last_idx = std::ssize(params) - 1;
  for (const auto& [idx, param] : params | std::views::enumerate) {
    buf += "- `";

    // TODO: cleanup builder from this
    if constexpr (std::is_same_v<TParamDescriptorNode, core::ast::nodes::FormalPar>) {
      if (param->direction) {
        buf += ast.Text(param->direction);
        buf += " ";
      }
      if (param->restriction) {
        buf += ast.Text(param->restriction);
        buf += " ";
      }
    }

    buf += ast.Text(param->type);
    buf += " ";
    buf += ast.Text(*param->name);

    if constexpr (std::is_same_v<TParamDescriptorNode, core::ast::nodes::Field>) {
      if (param->optional) {
        buf += " (optional)";
      }
    }
    if constexpr (std::is_same_v<TParamDescriptorNode, core::ast::nodes::FormalPar>) {
      if (param->value) {
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
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  const auto result = detail::FindSymbol(file, params.position);
  if (!result) {
    return nullptr;
  }
  const auto* n = result->node;
  const auto* sym = result->symbol;

  if (!sym || (sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
    return nullptr;
  }
  const auto* decl = sym->Declaration();
  const auto* provider_file = core::ast::utils::SourceFileOf(decl);

  auto& content = ctx->Temp<std::string>();
  content.reserve(256);  // TODO: check if it is justified actually

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
              .begin = m->nrange.begin,
              .end = m->body ? m->body->nrange.begin : m->nrange.end,
          }));
      break;
    }
    case core::ast::NodeKind::TemplateDecl: {
      const auto* m = decl->As<core::ast::nodes::TemplateDecl>();
      content += std::format(
          R"(
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
          m->params ? BuildMarkdownParameterList<core::ast::nodes::FormalPar>(provider_file->ast, m->params->list) : "",
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
    case core::ast::NodeKind::ClassTypeDecl: {
      const auto* m = decl->As<core::ast::nodes::ClassTypeDecl>();
      content += std::format(R"(
### class `{}`
---

```ttcn
{}
```
)",
                             provider_file->ast.Text(*m->name),
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
### enum `{}`
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
            buf.reserve(m->values.size() * 32);

            const auto last_idx = std::ssize(m->values) - 1;
            for (const auto& [idx, v] : m->values | std::views::enumerate) {
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
    case core::ast::NodeKind::Declarator: {
      const auto* m = decl->As<core::ast::nodes::Declarator>();
      const auto* vd = m->parent->As<core::ast::nodes::ValueDecl>();

      content += std::format(
          R"(
### {} `{}`

---

Type: `{}`

---

```ttcn
{} {} {}
```
)",
          provider_file->ast.Text(vd->kind->range),  //
          provider_file->ast.Text(*m->name),         //
          provider_file->ast.Text(vd->type),         //
          provider_file->ast.Text(core::ast::Range{
              .begin = vd->kind->range.begin,
              .end = vd->type->nrange.end,
          }),
          provider_file->ast.Text(*m->name),
          (vd->kind->kind == core::ast::TokenKind::CONST && m->value) ? provider_file->ast.Text(core::ast::Range{
                                                                            .begin = m->name->nrange.end + 1,
                                                                            .end = m->value->nrange.end,
                                                                        })
                                                                      : "");
      break;
    }
    case core::ast::NodeKind::FormalPar: {
      const auto* m = decl->As<core::ast::nodes::FormalPar>();
      content += std::format(R"(
### param `{}`

---

Type: `{}`

---

```ttcn
{}
```
)",
                             provider_file->ast.Text(*m->name),  //
                             provider_file->ast.Text(m->type),   //
                             provider_file->ast.Text(m),
                             provider_file->ast.Text(core::ast::Range{
                                 .begin = m->nrange.begin,
                                 .end = m->name->nrange.end,
                             }));
      break;
    }
    case core::ast::NodeKind::Field: {
      const auto* m = decl->As<core::ast::nodes::Field>();

      std::string prefix;
      if (m->parent->nkind == core::ast::NodeKind::StructTypeDecl) {
        prefix = std::format("{}::", provider_file->ast.Text(*m->parent->As<core::ast::nodes::StructTypeDecl>()->name));
      }

      content += std::format(
          R"(
### field `{}{}`

---

Type: `{}`

---

```ttcn
{}
```
)",
          prefix,
          provider_file->ast.Text(*m->name),  //
          provider_file->ast.Text(*m->type),  //
          provider_file->ast.Text(m));
      break;
    }
    case core::ast::NodeKind::ImportDecl: {
      const auto* m = decl->As<core::ast::nodes::ImportDecl>();
      if (n->parent->nkind == core::ast::NodeKind::ImportDecl) {
        m = n->parent->As<core::ast::nodes::ImportDecl>();
      }

      const auto* module = project.program.GetModule(provider_file->ast.Text(*m->module));
      if (!module) {
        break;
      }

      content += std::format("### module `{}`",
                             provider_file->ast.Text(*m->module),  //
                             provider_file->ast.Text(m));

      if (!m->list.empty() && m->list[0]->kind.kind == core::ast::TokenKind::IMPORT) {
        content += R"(
---

Transitively imports modules:
)";
        for (const auto& [import, descriptor] : module->imports) {
          if (descriptor.transit) {
            continue;
          }
          content += std::format("- `{}`\n", import);
        }
        content += "\n---\n";
      }

      content += std::format("\n---\n`{}`", module->sf->path);
      break;
    }
    default:
      const auto* parent = decl->parent;
      if (parent->nkind == core::ast::NodeKind::EnumTypeDecl ||
          (parent->nkind == core::ast::NodeKind::CallExpr &&
           parent->parent->nkind == core::ast::NodeKind::EnumTypeDecl)) {
        const auto* enumdecl = ((parent->nkind == core::ast::NodeKind::EnumTypeDecl) ? parent : parent->parent)
                                   ->As<core::ast::nodes::EnumTypeDecl>();
        const auto* valname = parent->nkind == core::ast::NodeKind::CallExpr
                                  ? parent->As<core::ast::nodes::CallExpr>()->fun->As<core::ast::Node>()
                                  : decl->As<core::ast::Node>();
        const auto* valdecl = (parent->nkind == core::ast::NodeKind::EnumTypeDecl) ? decl : parent;
        content += std::format(R"(
### enum value `{}::{}`

---

```ttcn
{}
```
)",
                               provider_file->ast.Text(*enumdecl->name),  //
                               provider_file->ast.Text(valname),          //
                               provider_file->ast.Text(valdecl));
        break;
      }
      content += "TODO";
      break;
  }

  if (provider_file != file) {
    const auto source_loc = conv::ToLSPPosition(provider_file->ast.lines.Translate(decl->nrange.begin));
    content += std::format(R"(
---

[module {}]({}#L{}C{}))",
                           provider_file->module->name, ctx->PathToFileUri(provider_file->path), source_loc.line + 1,
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
