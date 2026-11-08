#pragma once

#include <LSProtocol.h>

#include <vanadium/core/Program.h>
#include <vanadium/lib/FunctionRef.h>

#include "vanadium/ls/LanguageServerSession.h"

namespace vanadium::ls::actions {

struct AddMissingArguments {
  static void ResolveDiagnostic(std::vector<lsp::CodeAction>& result, const lsp::CodeActionParams&,
                                const lsp::Diagnostic&, const core::SourceFile&, LsSessionRef&);

  // TODO: lazy two-step rezolution with codeAction/resolve
};

}  // namespace vanadium::ls::actions
