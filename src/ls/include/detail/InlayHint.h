#pragma once

#include <vanadium/core/Program.h>

#include "LSProtocol.h"
#include "LanguageServerSession.h"

namespace vanadium::ls {
namespace detail {

[[nodiscard]] std::vector<lsp::InlayHint> CollectInlayHints(const lsp::InlayHintParams&, const core::SourceFile&,
                                                            LsSessionRef);

[[nodiscard]] std::optional<lsp::InlayHint> ResolveInlayHint(const lsp::InlayHint& original_hint, LsSessionRef);

}  // namespace detail
}  // namespace vanadium::ls
