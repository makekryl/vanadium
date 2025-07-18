#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "domain/LanguageServerSymbolDef.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {

template <>
rpc::ExpectedResult<lsp::DefinitionResult> methods::textDocument::definition::operator()(
    LsContext& ctx, const lsp::DefinitionParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  const auto* n = core::ast::utils::GetNodeAt(file->ast, file->ast.lines.GetPosition(core::ast::Location{
                                                             .line = params.position.line,
                                                             .column = params.position.character,
                                                         }));

  std::println(stderr, "NODE: '{}'", n->On(file->ast.src));

  if (n->nkind != core::ast::NodeKind::Ident) {
    return nullptr;
  }

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, n);
  const auto* sym = scope->Resolve(n->On(file->ast.src));
  if (!sym || (sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
    return nullptr;
  }

  const auto* decl = domain::GetReadableDeclaration(sym->Declaration());
  const auto* target_file = core::ast::utils::SourceFileOf(decl);
  const auto& uri = *ctx->GetTemporaryArena().Alloc<std::string>(ctx->PathToFileUri(target_file->path));
  return lsp::Location{
      .uri = uri,
      .range = conv::ToLSPRange(decl->nrange, target_file->ast),
  };
}
}  // namespace vanadium::ls
