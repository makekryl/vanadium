#pragma once

#include <string_view>

#include "LSProtocol.h"
#include "LanguageServerSession.h"
#include "Program.h"
#include "Semantic.h"

namespace vanadium::ls {
namespace detail {

namespace codeAction {
constexpr std::string_view kPayloadKeyUnresolved = "f";
constexpr std::string_view kPayloadKeyAutofix = "a";  // TODO: glz::json_t wrapper
}  // namespace codeAction

[[nodiscard]] lsp::CodeActionResult ProvideCodeActions(const lsp::CodeActionParams&, const core::SourceFile&,
                                                       LsSessionRef);

}  // namespace detail
}  // namespace vanadium::ls
