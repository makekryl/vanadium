// clang-format off
#include <LSProtocolGeneric.h>
// clang-format on

#include "vanadium/ls/detail/CodeAction.h"

#include <LSProtocol.h>

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/lib/ArenaString.h>
#include <vanadium/lib/Metaprogramming.h>

#include "vanadium/ls/LanguageServerLogger.h"
#include "vanadium/ls/actions/LintAutofix.h"
#include "vanadium/ls/actions/LsAction.h"
#include "vanadium/ls/actions/UnresolvedSymbolLookup.h"

namespace vanadium::ls::detail {

using Actions = mp::Typelist<         //
    actions::UnresolvedSymbolLookup,  //
    actions::LintAutofix              //
    >;

lsp::CodeActionResult ProvideCodeActions(const lsp::CodeActionParams& params, const core::SourceFile& file,
                                         LsSessionRef d) {
  std::vector<lsp::CodeAction> result;
  for (const auto& diag : params.context.diagnostics) {
    if (diag.data) {
      VLS_DEBUG("CA data='{}'", diag.data->dump().value());
    }
    Actions::Apply([&]<typename A>() {
      if constexpr (actions::CanResolveDiagnostic<A>) {
        A::ResolveDiagnostic(result, params, diag, file, d);
      }
    });
  }
  return result;
}

}  // namespace vanadium::ls::detail
