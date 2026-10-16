#include "vanadium/ls/detail/References.h"

#include <string>
#include <unordered_map>

#include <LSProtocol.h>
#include <LSProtocolEx.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerConv.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/LanguageServerSession.h"
#include "vanadium/ls/detail/Definition.h"

// TODO:
//  1) support fields
//  2) look for references in other files

namespace vanadium::ls {

namespace {
lsp::ReferencesResult CollectReferences(const lsp::ReferenceParams& params, const core::SourceFile& file,
                                        LsSessionRef d) {
  const auto symres = detail::FindSymbol(&file, params.position);
  if (!symres) {
    return {};
  }

  auto& file_uri_map = *d.arena.Alloc<std::unordered_map<const core::SourceFile*, std::string>>();

  std::vector<lsp::Location> refs;
  detail::VisitReferences(&file, *symres,
                          detail::ReferencesVisitParams{
                              .global_search = true,
                              .include_declaration = params.context.includeDeclaration,
                          },
                          [&](const core::SourceFile* sf, const ast::Node* m) {
                            auto [it, inserted] = file_uri_map.try_emplace(sf);
                            if (inserted) {
                              it->second = PathToFileUri(d.solution, sf->path);
                            }
                            refs.emplace_back(lsp::Location{
                                .uri = it->second,
                                .range = conv::ToLSPRange(m->nrange, sf->ast),
                            });
                          });

  return refs;
}
}  // namespace

rpc::ExpectedResult<lsp::ReferencesResult> methods::textDocument::references::invoke(
    LsContext& ctx, const lsp::ReferenceParams& params) {
  return ctx.WithFile<lsp::ReferencesResult>(params, CollectReferences).value_or(nullptr);
}

}  // namespace vanadium::ls
