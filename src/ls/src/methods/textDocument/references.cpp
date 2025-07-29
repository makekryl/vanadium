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
  auto res = ctx->WithFile<lsp::ReferencesResult>(
      params.textDocument.uri, [&](const core::Program&, const core::SourceFile& file) -> lsp::ReferencesResult {
        std::vector<lsp::Location> refs;
        detail::VisitLocalReferences(&file, params.position, params.context.includeDeclaration,
                                     [&](const core::ast::nodes::Ident* ident) {
                                       refs.emplace_back(lsp::Location{
                                           .uri = params.textDocument.uri,
                                           .range = conv::ToLSPRange(ident->nrange, file.ast),
                                       });
                                     });

        return refs;
      });
  return res.value_or(nullptr);
}
}  // namespace vanadium::ls
