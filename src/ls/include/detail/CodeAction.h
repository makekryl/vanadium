#pragma once

#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>

#include <string_view>

#include "LSProtocol.h"
#include "LanguageServerSession.h"

namespace vanadium::ls {
namespace detail {

namespace codeAction {
const char* const kPayloadKeyUnresolved = "f";
const char* const kPayloadKeyAutofix = "a";  // TODO: glz::generic wrapper
}  // namespace codeAction

[[nodiscard]] lsp::CodeActionResult ProvideCodeActions(const lsp::CodeActionParams&, const core::SourceFile&,
                                                       LsSessionRef);

}  // namespace detail
}  // namespace vanadium::ls
