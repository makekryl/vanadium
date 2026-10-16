#pragma once

#include <LSProtocol.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>
#include <vanadium/lib/FunctionRef.h>

#include "vanadium/ls/detail/Definition.h"

namespace vanadium::ls::detail {

struct ReferencesVisitParams {
  bool global_search;
  bool include_declaration;
};

void VisitReferences(const core::SourceFile*, SymbolSearchResult, ReferencesVisitParams,
                     lib::Consumer<const core::SourceFile*, const ast::Node*>);

}  // namespace vanadium::ls::detail
