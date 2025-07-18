#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>
#include <optional>

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

namespace {
std::optional<std::string_view> LookupTypeName(const core::semantic::Symbol* sym) {
  const auto* decl = sym->Declaration();

  const core::ast::Node* typename_node{nullptr};

  switch (decl->nkind) {
    case core::ast::NodeKind::Declarator: {
      const auto* valdecl = decl->parent->As<core::ast::nodes::ValueDecl>();
      typename_node = valdecl->type;
      break;
    }
    case core::ast::NodeKind::FormalPar: {
      const auto* valdecl = decl->As<core::ast::nodes::FormalPar>();
      typename_node = valdecl->type;
      break;
    }
    default: {
      std::fprintf(stderr, "LookupTypeName :: not supported\n");
      return std::nullopt;
    }
  }

  const auto* decl_file = core::ast::utils::SourceFileOf(decl);
  return decl_file->Text(typename_node);
}
const core::semantic::Symbol* LookupTypeSymbol(const core::semantic::Symbol* sym) {
  if (!(sym->Flags() & (core::semantic::SymbolFlags::kVariable | core::semantic::SymbolFlags::kArgument |
                        core::semantic::SymbolFlags::kField))) {
    // TODO: support chained expressions
    return nullptr;
  }

  const auto* decl = sym->Declaration()->parent;

  const core::ast::Node* typename_node{nullptr};

  switch (decl->nkind) {
    case core::ast::NodeKind::Declarator: {
      const auto* valdecl = decl->parent->As<core::ast::nodes::ValueDecl>();
      typename_node = valdecl->type;
      break;
    }
    case core::ast::NodeKind::FormalPar: {
      const auto* valdecl = decl->As<core::ast::nodes::FormalPar>();
      typename_node = valdecl->type;
      break;
    }
    default: {
      std::fprintf(stderr, "LookupTypeName :: not supported\n");
      return nullptr;
    }
  }

  const auto* decl_file = core::ast::utils::SourceFileOf(decl);
  const auto typename_str = decl_file->Text(typename_node);

  return decl_file->module->scope->Resolve(typename_str);
}
}  // namespace

template <>
rpc::ExpectedResult<lsp::TypeDefinitionResult> methods::textDocument::typeDefinition::operator()(
    LsContext& ctx, const lsp::TypeDefinitionParams& params) {
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
  const auto* sym = scope->Resolve(n->On(file->ast.src));
  if (!sym) {
    return nullptr;
  }

  const auto* type_sym = LookupTypeSymbol(sym);
  if (!type_sym) {
    std::fprintf(stderr, "not type_sym\n");
    return nullptr;
  }

  const auto* type_decl = domain::GetReadableDeclaration(type_sym->Declaration());
  const auto* type_file = core::ast::utils::SourceFileOf(type_decl);

  const auto& uri = *ctx->GetTemporaryArena().Alloc<std::string>(ctx->PathToFileUri(type_file->path));
  return lsp::Location{
      .uri = uri,
      .range = conv::ToLSPRange(type_decl->nrange, type_file->ast),
  };
}
}  // namespace vanadium::ls
