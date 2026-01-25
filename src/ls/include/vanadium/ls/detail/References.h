#pragma once

#include <LSProtocol.h>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>
#include <vanadium/lib/FunctionRef.h>

namespace vanadium::ls::detail {

void VisitLocalReferences(const core::SourceFile*, lsp::Position, bool include_decl,
                          lib::Consumer<const ast::nodes::Ident*>);

}
