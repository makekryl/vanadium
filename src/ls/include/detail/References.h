#pragma once

#include "ASTNodes.h"
#include "FunctionRef.h"
#include "LSProtocol.h"
#include "Program.h"

namespace vanadium::ls::detail {

void VisitLocalReferences(const core::SourceFile*, lsp::Position, bool include_decl,
                          lib::Consumer<const core::ast::nodes::Ident*>);

}
