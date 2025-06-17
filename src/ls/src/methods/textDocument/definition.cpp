#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "ASTTypes.h"
#include "LSProtocol.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"
#include "utils/ASTUtils.h"
#include "utils/SemanticUtils.h"

static std::string lsptoreal(std::string_view lsppath) {
  constexpr std::size_t prefixsize = std::string_view{"file://"}.size();
  return std::string(lsppath.substr(prefixsize, lsppath.size() - prefixsize));
}

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::DefinitionResult> methods::textDocument::definition::operator()(
    LsContext& ctx, const lsp::DefinitionParams& params) {
  const auto path = ctx->project->RelativizePath(lsptoreal(std::string(params.textDocument.uri)));
  const auto* file = ctx->program.GetFile(path);

  std::println(stderr, "getpos(line={}, col={})", params.position.line, params.position.character);

  const auto* n = core::ast::utils::GetNodeAt(file->ast.lines.GetPosition(core::ast::Location{
                                                  .line = params.position.line,
                                                  .column = params.position.character,
                                              }),
                                              file->ast);
  const core::semantic::Scope* scope = core::semantic::utils::FindScope(file->module->scope, n);

  if (const auto* sym = scope->Resolve(n->On(file->ast.src)); sym) {
    return lsp::Location{
        .uri = params.textDocument.uri,
        .range =
            lsp::Range{
                .start = conv::ToLSPPosition(file->ast.lines.Translate(sym->Declaration()->parent->nrange.begin)),
                .end = conv::ToLSPPosition(file->ast.lines.Translate(sym->Declaration()->parent->nrange.end)),
            },
    };
  }

  return nullptr;
}
}  // namespace vanadium::ls
