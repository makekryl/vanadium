#include "detail/References.h"

#include <glaze/ext/jsonrpc.hpp>
#include <glaze/json/write.hpp>

#include "ASTNodes.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "Semantic.h"

// TODO:
//  1) support fields
//  2) look for references in other files

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::ReferencesResult> methods::textDocument::references::operator()(
    LsContext& ctx, const lsp::ReferenceParams& params) {
  const auto& [project, path] = ctx->ResolveFile(params.textDocument.uri);
  const auto* file = project.program.GetFile(path);

  std::vector<lsp::Location> refs;
  detail::VisitLocalReferences(file, params.position, params.context.includeDeclaration,
                               [&](const core::ast::nodes::Ident* ident) {
                                 refs.emplace_back(lsp::Location{
                                     .uri = params.textDocument.uri,
                                     .range = conv::ToLSPRange(ident->nrange, file->ast),
                                 });
                               });

  return refs;
}
}  // namespace vanadium::ls
