#pragma once

#include "ASTNodes.h"
#include "Arena.h"
#include "LSProtocol.h"
#include "Program.h"
#include "Semantic.h"
#include "Solution.h"

namespace vanadium::ls {
namespace detail {

void ComputeInlayHint(const core::SourceFile*, const core::semantic::Scope*, const core::ast::Node*, lib::Arena&,
                      std::vector<lsp::InlayHint>& out);
[[nodiscard]] std::optional<lsp::InlayHint> ResolveInlayHint(const tooling::Solution&, lib::Arena&,
                                                             const lsp::InlayHint& original_hint);

}  // namespace detail
}  // namespace vanadium::ls
