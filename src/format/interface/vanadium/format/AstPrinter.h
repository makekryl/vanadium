#pragma once

#include <cstddef>
#include <string>

namespace vanadium {

namespace ast {
struct AST;
struct Node;
}  // namespace ast

namespace format {

struct PrintOptions {
  std::size_t tab_width;
  std::size_t print_width;

  std::size_t max_newlines{3};
};

std::string PrintAst(const ast::AST& ast, const ast::Node*, PrintOptions);

}  // namespace format

}  // namespace vanadium
