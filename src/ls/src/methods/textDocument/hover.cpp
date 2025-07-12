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
std::string BuildMarkdownParameterList(const core::ast::AST& ast, const core::ast::nodes::FormalPars* pars) {
  std::string buf;
  buf.reserve(pars->list.size() * 32);

  const auto last_idx = static_cast<decltype(pars->list)::difference_type>(pars->list.size()) - 1;
  for (const auto& [idx, par] : pars->list | std::views::enumerate) {
    buf += "- `";
    buf += ast.Text(par->type);
    buf += " ";
    buf += ast.Text(*par->name);
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
### function `{}`
---
→ `{}`
{}
---
```ttcn
{}
```
)",
          provider_file->ast.Text(*m->name),  //
          m->ret ? provider_file->ast.Text(m->ret->type) : "void",
          m->params->list.empty()
              ? ""
              : std::format("\nParameters:\n{}", BuildMarkdownParameterList(provider_file->ast, m->params)),
          provider_file->ast.Text(core::ast::Range{
              .begin = decl->nrange.begin,
              .end = m->body->nrange.begin,
          }));
      break;
    }
    case core::ast::NodeKind::TemplateDecl: {
      const auto* m = decl->As<core::ast::nodes::TemplateDecl>();
      content += std::format(R"(
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
                             BuildMarkdownParameterList(provider_file->ast, m->params),
                             provider_file->ast.Text(core::ast::Range{
                                 .begin = decl->nrange.begin,
                                 .end = m->value->nrange.begin,
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
