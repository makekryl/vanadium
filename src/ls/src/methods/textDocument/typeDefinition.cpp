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
#include "TypeChecker.h"
#include "detail/Definition.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {

template <>
rpc::ExpectedResult<lsp::TypeDefinitionResult> methods::textDocument::typeDefinition::operator()(
    LsContext& ctx, const lsp::TypeDefinitionParams& params) {
  // TODO: shorten the handler, use ctx->WithFile
  const auto& resolution = ctx->ResolveFile(params.textDocument.uri);
  if (!resolution) {
    return nullptr;
  }
  const auto& [project, path] = *resolution;
  const auto* file = project.program.GetFile(path);

  const auto result = detail::FindSymbol(file, params.position);
  if (!result) {
    return nullptr;
  }
  const auto* sym = result->symbol;

  if (!sym || (sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
    return nullptr;
  }
  const auto* decl = sym->Declaration()->As<core::ast::nodes::Decl>();
  const auto* provider_file = core::ast::utils::SourceFileOf(decl);

  const auto* type_sym = core::checker::ResolveDeclarationType(
      provider_file, core::semantic::utils::FindScope(provider_file->module->scope, decl), decl);
  if (!type_sym) {
    return nullptr;
  }

  const auto* type_decl = detail::GetReadableDefinition(type_sym->Declaration());
  const auto* type_file = core::ast::utils::SourceFileOf(type_decl);

  const auto& uri = ctx->Temp<std::string>(ctx->PathToFileUri(type_file->path));
  return lsp::Location{
      .uri = uri,
      .range = conv::ToLSPRange(type_decl->nrange, type_file->ast),
  };
}
}  // namespace vanadium::ls
