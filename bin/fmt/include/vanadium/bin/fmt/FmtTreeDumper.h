#pragma once

#include <ostream>

#include <vanadium/ast/AST.h>

namespace vanadium::bin::fmt {
void DumpSerializedTree(std::ostream&, const ast::AST&, const ast::Node*);
}
