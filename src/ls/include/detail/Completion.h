#pragma once

#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>

#include "LSProtocol.h"
#include "LanguageServerSession.h"

namespace vanadium::ls {
namespace detail {

[[nodiscard]] lsp::CompletionList CollectCompletions(const lsp::CompletionParams&, const core::SourceFile&,
                                                     LsSessionRef);
[[nodiscard]] std::optional<lsp::CompletionItem> ResolveCompletionItem(const lsp::CompletionItem& original_completion,
                                                                       LsSessionRef);

}  // namespace detail
}  // namespace vanadium::ls
