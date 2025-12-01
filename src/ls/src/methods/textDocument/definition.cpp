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

namespace {
lsp::DefinitionResult ProvideDefinition(const lsp::DefinitionParams& params, const core::SourceFile& file,
                                        LsSessionRef d) {
  const auto symres = detail::FindSymbol(&file, params.position);
  if (!symres) {
    return nullptr;
  }
  const auto* sym = symres->type.sym;

  if (!sym || (sym->Flags() & (core::semantic::SymbolFlags::kBuiltin | core::semantic::SymbolFlags::kBuiltin))) {
    return nullptr;
  }
  if (sym->Flags() & core::semantic::SymbolFlags::kImportedModule) {
    const auto* module = file.program->GetModule(sym->GetName());
    if (!module) {
      return nullptr;
    }
    return lsp::Location{
        .uri = *d.arena.Alloc<std::string>(PathToFileUri(d.solution, module->sf->path)),
        .range = {},
    };
  }

  const auto* decl = sym->Declaration();
  const auto* provider_file = ast::utils::SourceFileOf(decl);

  const auto& uri = *d.arena.Alloc<std::string>(PathToFileUri(d.solution, provider_file->path));
  return lsp::Location{
      .uri = uri,
      .range = conv::ToLSPRange(detail::GetReadableDefinition(decl)->nrange, provider_file->ast),
  };
}
}  // namespace

template <>
rpc::ExpectedResult<lsp::DefinitionResult> methods::textDocument::definition::invoke(
    LsContext& ctx, const lsp::DefinitionParams& params) {
  return ctx.WithFile<lsp::DefinitionResult>(params, ProvideDefinition).value_or(nullptr);
}
}  // namespace vanadium::ls
