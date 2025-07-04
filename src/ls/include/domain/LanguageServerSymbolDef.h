#pragma once

#include "ASTNodes.h"
#include "Semantic.h"

namespace vanadium::ls {
namespace domain {

const core::ast::Node* GetReadableDeclaration(const core::semantic::Symbol* sym);

}  // namespace domain
}  // namespace vanadium::ls
