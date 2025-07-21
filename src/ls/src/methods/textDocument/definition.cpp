#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "detail/LanguageServerSymbolDef.h"
#include "utils/ASTUtils.h"

namespace vanadium::ls {

template <>
rpc::ExpectedResult<lsp::DefinitionResult> methods::textDocument::definition::operator()(
    LsContext& ctx, const lsp::DefinitionParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  const auto result = detail::FindSymbol(file, params.position);
  if (!result) {
    return nullptr;
  }
  const auto* sym = result->symbol;

  if (!sym || (sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
    return nullptr;
  }
  if (sym->Flags() & core::semantic::SymbolFlags::kImportedModule) {
    const auto* module = project.program.GetModule(sym->GetName());
    if (!module) {
      return nullptr;
    }
    return lsp::Location{
        .uri = ctx->Temp<std::string>(ctx->PathToFileUri(module->sf->path)),
        .range = {},
    };
  }

  const auto* decl = sym->Declaration();
  const auto* provider_file = core::ast::utils::SourceFileOf(decl);

  const auto& uri = ctx->Temp<std::string>(ctx->PathToFileUri(provider_file->path));
  return lsp::Location{
      .uri = uri,
      .range = conv::ToLSPRange(decl->nrange, provider_file->ast),
  };
}
}  // namespace vanadium::ls
