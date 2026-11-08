#include "vanadium/ls/actions/LintAutofix.h"

#include <string_view>

#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Program.h>
#include <vanadium/lib/ArenaString.h>
#include <vanadium/lib/FunctionRef.h>
#include <vanadium/lib/Metaprogramming.h>

#include "vanadium/ls/LanguageServerConv.h"
#include "vanadium/ls/detail/CodeAction.h"

namespace vanadium::ls::actions {

void LintAutofix::ResolveDiagnostic(std::vector<lsp::CodeAction>& result, const lsp::CodeActionParams& params,
                                    const lsp::Diagnostic& diag, const core::SourceFile& file, LsSessionRef&) {
  if (!diag.data || !diag.data->is_object()) {
    return;
  }
  const auto& data = diag.data->get_object();

  const auto& it = diag.data->get_object().find(detail::codeAction::kPayloadKeyAutofix);
  if (it == data.end()) {
    return;
  }

  const auto& af = it->second;
  const auto& serialized_range = af.at("range");
  result.emplace_back(
      lsp::CodeAction{
          .title = af.at("title").as<std::string_view>(),
          .kind = lsp::CodeActionKind::kQuickfix,
          .isPreferred = true,
          .edit =
              lsp::WorkspaceEdit{
                  .changes = {{{params.textDocument.uri,
                                {lsp::TextEdit{
                                    .range = lsp::Range{.start = conv::ToLSPPosition(
                                                            file.ast.lines.Translate(serialized_range[0].as<int>())),
                                                        .end = conv::ToLSPPosition(
                                                            file.ast.lines.Translate(serialized_range[1].as<int>()))},
                                    .newText = af.at("replacement").as<std::string_view>(),
                                }}}}},
              },
      });
}

}  // namespace vanadium::ls::actions
