#pragma once

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/lib/FunctionRef.h>

#include "LSProtocol.h"
#include "Program.h"

namespace vanadium::ls::detail {

void VisitLocalReferences(const core::SourceFile*, lsp::Position, bool include_decl,
                          lib::Consumer<const ast::nodes::Ident*>);

}
