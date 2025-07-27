#pragma once

#include "AST.h"
#include "ASTNodes.h"

namespace vanadium::ls {
namespace detail {

[[nodiscard]] core::ast::pos_t FindModuleBodyStart(const core::ast::AST&, const core::ast::nodes::Module*);
[[nodiscard]] core::ast::pos_t FindPositionAfterLastImport(const core::ast::AST&);

}  // namespace detail
}  // namespace vanadium::ls
