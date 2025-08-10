#pragma once

#include "LSProtocol.h"
#include "LanguageServerSession.h"
#include "Program.h"
#include "Semantic.h"

namespace vanadium::ls {
namespace detail {

[[nodiscard]] std::vector<lsp::CompletionItem> CollectCompletions(const lsp::CompletionParams&, const core::SourceFile&,
                                                                  LsSessionRef);
[[nodiscard]] std::optional<lsp::CompletionItem> ResolveCompletionItem(const lsp::CompletionItem& original_completion,
                                                                       LsSessionRef);

}  // namespace detail
}  // namespace vanadium::ls
