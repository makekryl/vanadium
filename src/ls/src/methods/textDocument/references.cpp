#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "detail/LanguageServerSymbolDef.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

// TODO:
//  1) support fields
//  2) look for references in other files

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::ReferencesResult> methods::textDocument::references::operator()(
    LsContext& ctx, const lsp::ReferenceParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  const auto* n = core::ast::utils::GetNodeAt(file->ast, file->ast.lines.GetPosition(core::ast::Location{
                                                             .line = params.position.line,
                                                             .column = params.position.character,
                                                         }));

  if (n->nkind != core::ast::NodeKind::Ident) {
    return nullptr;
  }

  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, n);

  const auto sym_name = n->On(file->ast.src);
  const auto* sym = scope->Resolve(sym_name);
  if (!sym || !(sym->Flags() & (core::semantic::SymbolFlags::kVariable | core::semantic::SymbolFlags::kArgument))) {
    // TODO: support datatypes
    return nullptr;
  }

  std::vector<lsp::Location> refs;

  if (params.context.includeDeclaration) {
    const auto* decl = detail::GetReadableDeclaration(sym->Declaration());
    const auto* target_file = core::ast::utils::SourceFileOf(decl);
    const auto& uri = ctx->Temp<std::string>(ctx->PathToFileUri(target_file->path));
    refs.emplace_back(lsp::Location{
        .uri = uri,
        .range = conv::ToLSPRange(n->nrange, target_file->ast),
    });
  }

  scope->Container()->Accept([&](const core::ast::Node* vn) {
    bool should_introspect{true};
    if (vn->nkind == core::ast::NodeKind::AssignmentExpr &&
        (vn->parent->nkind == core::ast::NodeKind::CompositeLiteral ||
         vn->parent->nkind == core::ast::NodeKind::ParenExpr)) {
      should_introspect = false;
      vn = vn->As<core::ast::nodes::AssignmentExpr>()->value;
    }
    if (vn->nkind == core::ast::NodeKind::Ident &&
        (vn->parent->nkind != core::ast::NodeKind::SelectorExpr ||
         vn->parent->As<core::ast::nodes::SelectorExpr>()->x == vn) &&
        file->ast.Text(vn) == sym_name) {
      refs.emplace_back(lsp::Location{
          .uri = params.textDocument.uri,
          .range = conv::ToLSPRange(vn->nrange, file->ast),
      });
    }
    return should_introspect;
  });

  return refs;

  return nullptr;
}
}  // namespace vanadium::ls
