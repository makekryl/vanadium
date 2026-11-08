#pragma once

#include <LSProtocol.h>

#include <vanadium/core/Program.h>
#include <vanadium/lib/FunctionRef.h>

#include "vanadium/ls/LanguageServerSession.h"

namespace vanadium::ls::actions {

template <typename T>
concept CanResolveDiagnostic = requires {
  {
    T::ResolveDiagnostic(                               //
        std::declval<std::vector<lsp::CodeAction>&>(),  //
        std::declval<const lsp::CodeActionParams&>(),   //
        std::declval<const lsp::Diagnostic&>(),         //
        std::declval<const core::SourceFile&>(),        //
        std::declval<LsSessionRef&>()                   //
    )
  } -> std::same_as<void>;
};

}  // namespace vanadium::ls::actions
