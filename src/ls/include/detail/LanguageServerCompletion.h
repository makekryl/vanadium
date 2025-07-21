#pragma once

#include "ASTNodes.h"
#include "Arena.h"
#include "LSProtocol.h"
#include "Program.h"
#include "Semantic.h"

namespace vanadium::ls {
namespace detail {

void PerformCompletion(const core::SourceFile*, const core::ast::Node*, lib::Arena&, std::vector<lsp::CompletionItem>&,
                       std::size_t limit);

}  // namespace detail
}  // namespace vanadium::ls
