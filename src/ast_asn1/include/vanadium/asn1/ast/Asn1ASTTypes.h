#pragma once

#include <vanadium/ast/ASTTypes.h>

#include "FunctionRef.h"

namespace vanadium::asn1::ast {

using vanadium::ast::LineMapping;
using vanadium::ast::Location;
using vanadium::ast::pos_t;
using vanadium::ast::Range;
using vanadium::ast::SyntaxError;

struct Node;

using NodeInspector = lib::Predicate<const Node*>;
void Inspect(const Node*, const NodeInspector&);

};  // namespace vanadium::asn1::ast
