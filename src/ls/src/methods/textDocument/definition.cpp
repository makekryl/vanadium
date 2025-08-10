#include "detail/Definition.h"

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"

namespace vanadium::ls {

template <>
rpc::ExpectedResult<lsp::DefinitionResult> methods::textDocument::definition::operator()(
    LsContext& ctx, const lsp::DefinitionParams& params) {
  auto res = ctx->WithFile<lsp::DefinitionResult>(
      params, [&](const auto&, const core::SourceFile& file, LsSessionRef) -> lsp::DefinitionResult {
        const auto result = detail::FindSymbol(&file, params.position);
        if (!result) {
          return nullptr;
        }
        const auto* sym = result->symbol;

        if (!sym ||
            (sym->Flags() & (core::semantic::SymbolFlags::kBuiltin | core::semantic::SymbolFlags::kBuiltinDef))) {
          return nullptr;
        }
        if (sym->Flags() & core::semantic::SymbolFlags::kImportedModule) {
          const auto* module = file.program->GetModule(sym->GetName());
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
            .range = conv::ToLSPRange(detail::GetReadableDefinition(decl)->nrange, provider_file->ast),
        };
      });
  return res.value_or(nullptr);
}
}  // namespace vanadium::ls
