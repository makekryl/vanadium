#pragma once

#include "AST.h"
#include "ASTNodes.h"

namespace vanadium::ls {
namespace detail {

[[nodiscard]] ast::pos_t FindModuleBodyStart(const ast::AST&, const ast::nodes::Module*);
[[nodiscard]] ast::pos_t FindPositionAfterLastImport(const ast::AST&);

}  // namespace detail
}  // namespace vanadium::ls
