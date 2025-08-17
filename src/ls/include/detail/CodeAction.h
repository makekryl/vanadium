#pragma once

#include <string_view>

#include "LSProtocol.h"
#include "LanguageServerSession.h"
#include "Program.h"
#include "Semantic.h"

namespace vanadium::ls {
namespace detail {

namespace codeAction {
const char* const kPayloadKeyUnresolved = "f";
const char* const kPayloadKeyAutofix = "a";  // TODO: glz::json_t wrapper
}  // namespace codeAction

[[nodiscard]] lsp::CodeActionResult ProvideCodeActions(const lsp::CodeActionParams&, const core::SourceFile&,
                                                       LsSessionRef);

}  // namespace detail
}  // namespace vanadium::ls
