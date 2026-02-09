#pragma once

#include <string>

namespace vanadium {

namespace ast {
struct Node;
}

namespace format {

struct PrintOptions {
  std::size_t tab_width;
  std::size_t print_width;
};

std::string PrintAst(std::string_view src, const ast::Node*, PrintOptions);

}  // namespace format

}  // namespace vanadium
