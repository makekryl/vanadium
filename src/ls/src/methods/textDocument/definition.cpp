#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <memory>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {

namespace {
const core::ast::Node* GetReadableDeclaration(const core::semantic::Symbol* sym) {
  const auto* n = sym->Declaration();
  switch (n->nkind) {
    case core::ast::NodeKind::FuncDecl:
      return std::addressof(*(n->As<core::ast::nodes::FuncDecl>()->name));
    default:
      return n;
  }
}
}  // namespace

template <>
rpc::ExpectedResult<lsp::DefinitionResult> methods::textDocument::definition::operator()(
    LsContext& ctx, const lsp::DefinitionParams& params) {
  const auto path = ctx->FileUriToPath(params.textDocument.uri);
  const auto* file = ctx->program.GetFile(path);

  const auto* n = core::ast::utils::GetNodeAt(file->ast, file->ast.lines.GetPosition(core::ast::Location{
                                                             .line = params.position.line,
                                                             .column = params.position.character,
                                                         }));

  std::println(stderr, "NODE: '{}'", n->On(file->ast.src));

  if (n->nkind != core::ast::NodeKind::Ident) {
    return nullptr;
  }

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, n);

  if (const auto* sym = scope->Resolve(n->On(file->ast.src)); sym) {
    const auto* decl = GetReadableDeclaration(sym);
    const auto* target_file = core::ast::utils::SourceFileOf(decl);
    const auto& uri = *ctx->GetTemporaryArena().Alloc<std::string>(ctx->PathToFileUri(target_file->path));
    return lsp::Location{
        .uri = uri,
        .range =
            lsp::Range{
                .start = conv::ToLSPPosition(target_file->ast.lines.Translate(decl->nrange.begin)),
                .end = conv::ToLSPPosition(target_file->ast.lines.Translate(decl->nrange.end)),
            },
    };
  }

  return nullptr;
}
}  // namespace vanadium::ls
