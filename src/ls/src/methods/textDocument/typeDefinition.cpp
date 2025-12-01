#include <optional>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "Program.h"
#include "Semantic.h"
#include "TypeChecker.h"
#include "detail/Definition.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

namespace vanadium::ls {

namespace {
lsp::TypeDefinitionResult ProvideTypeDefinition(const lsp::TypeDefinitionParams& params, const core::SourceFile& file,
                                                LsSessionRef d) {
  const auto symres = detail::FindSymbol(&file, params.position);
  if (!symres) {
    return nullptr;
  }
  const auto* sym = symres->type.sym;

  if (!sym || (sym->Flags() & core::semantic::SymbolFlags::kBuiltin)) {
    return nullptr;
  }
  const auto* decl = sym->Declaration()->As<ast::nodes::Decl>();
  const auto* provider_file = ast::utils::SourceFileOf(decl);

  const auto* type_sym = core::checker::ResolveDeclarationType(provider_file, decl).sym;
  if (!type_sym) {
    return nullptr;
  }

  const auto* type_decl = detail::GetReadableDefinition(type_sym->Declaration());
  const auto* type_file = ast::utils::SourceFileOf(type_decl);

  return lsp::Location{
      .uri = *d.arena.Alloc<std::string>(PathToFileUri(d.solution, type_file->path)),
      .range = conv::ToLSPRange(type_decl->nrange, type_file->ast),
  };
}
}  // namespace

template <>
rpc::ExpectedResult<lsp::TypeDefinitionResult> methods::textDocument::typeDefinition::invoke(
    LsContext& ctx, const lsp::TypeDefinitionParams& params) {
  return ctx.WithFile<lsp::TypeDefinitionResult>(params, ProvideTypeDefinition).value_or(nullptr);
}
}  // namespace vanadium::ls
