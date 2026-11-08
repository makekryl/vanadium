#pragma once

#include <LSProtocol.h>

#include <vanadium/core/Program.h>
#include <vanadium/lib/FunctionRef.h>

#include "vanadium/ls/LanguageServerSession.h"

namespace vanadium::ls::actions {

struct LintAutofix {
  static void ResolveDiagnostic(std::vector<lsp::CodeAction>& result, const lsp::CodeActionParams&,
                                const lsp::Diagnostic&, const core::SourceFile&, LsSessionRef);
};

}  // namespace vanadium::ls::actions
