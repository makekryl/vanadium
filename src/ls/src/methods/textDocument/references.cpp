#include "detail/References.h"

#include <vanadium/ast/ASTNodes.h>

#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"
#include "LanguageServerConv.h"
#include "LanguageServerMethods.h"
#include "LanguageServerSession.h"
#include "Program.h"
#include "Semantic.h"

// TODO:
//  1) support fields
//  2) look for references in other files

namespace vanadium::ls {
template <>
rpc::ExpectedResult<lsp::ReferencesResult> methods::textDocument::references::invoke(
    LsContext& ctx, const lsp::ReferenceParams& params) {
  auto res = ctx.WithFile<lsp::ReferencesResult>(
      params, [&](const auto&, const core::SourceFile& file, LsSessionRef) -> lsp::ReferencesResult {
        std::vector<lsp::Location> refs;
        detail::VisitLocalReferences(&file, params.position, params.context.includeDeclaration,
                                     [&](const ast::nodes::Ident* ident) {
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
