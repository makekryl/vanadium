#pragma once

#include <string_view>

#include "ASTNodes.h"
#include "Arena.h"
#include "LSProtocol.h"
#include "Program.h"
#include "Semantic.h"
#include "Solution.h"

namespace vanadium::ls {
namespace detail {

void CollectCompletions(const core::SourceFile*, const core::ast::Node*, lib::Arena&, std::vector<lsp::CompletionItem>&,
                        std::size_t limit);
[[nodiscard]] std::optional<lsp::CompletionItem> ResolveCompletionItem(const tooling::Solution& solution, lib::Arena&,
                                                                       const lsp::CompletionItem& original_completion);

}  // namespace detail
}  // namespace vanadium::ls
