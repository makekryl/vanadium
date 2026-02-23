#pragma once

#include <cstddef>
#include <string>

namespace vanadium {

namespace lib {
class Arena;
}

namespace ast {
struct AST;
struct Node;
}  // namespace ast

namespace format {

struct PrintOptions {
  std::size_t tab_width{4};
  std::size_t print_width{80};

  std::size_t max_empty_newlines{2};
};

std::string PrintAst(const ast::AST& ast, const ast::Node*, lib::Arena&, PrintOptions);

}  // namespace format

}  // namespace vanadium
