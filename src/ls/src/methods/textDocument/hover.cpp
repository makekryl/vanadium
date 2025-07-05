#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolAdjustments.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::HoverResult> methods::textDocument::hover::operator()(LsContext& ctx,
                                                                               const lsp::HoverParams& params) {
  const auto path = ctx->FileUriToPath(params.textDocument.uri);
  const auto* file = ctx->program.GetFile(path);

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
      content += std::format(R"(
### function `{}`
---
→ `{}`\
Parameters:
{}
---
```ttcn
{}
```
)",
                             provider_file->ast.Text(std::addressof(*m->name)),
                             m->ret ? provider_file->ast.Text(m->ret->type) : "none", "- `test`",
                             provider_file->ast.Text(core::ast::Range{
                                 .begin = decl->nrange.begin,
                                 .end = m->body->nrange.begin,
                             }));
      break;
    }
    case core::ast::NodeKind::TemplateDecl:
      content += std::format(R"(
### template `{}`
---
```ttcn
{}
```
)",
                             provider_file->ast.Text(std::addressof(*decl->As<core::ast::nodes::TemplateDecl>()->name)),
                             provider_file->ast.Text(core::ast::Range{
                                 .begin = decl->nrange.begin,
                                 .end = decl->As<core::ast::nodes::TemplateDecl>()->value->nrange.begin,
                             }));
      break;
    default:
      content += "TODO";
      break;
  }

  if (provider_file != file) {
    const auto source_loc = provider_file->ast.lines.Translate(decl->nrange.begin);
    content += std::format(R"(
---
[module {}]({}#L{}C{}))",
                           provider_file->module->name, ctx->PathToFileUri(provider_file->path), source_loc.line,
                           source_loc.column);
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
