#pragma once

#include "ASTTypes.h"
#include "Arena.h"
#include "LSProtocol.h"
#include "Program.h"
#include "Semantic.h"
#include "Solution.h"

namespace vanadium::ls {
namespace detail {

[[nodiscard]] std::vector<lsp::InlayHint> CollectInlayHints(const core::SourceFile*, const core::ast::Range&,
                                                            lib::Arena&);

[[nodiscard]] std::optional<lsp::InlayHint> ResolveInlayHint(const tooling::Solution&, lib::Arena&,
                                                             const lsp::InlayHint& original_hint);

}  // namespace detail
}  // namespace vanadium::ls
