
#include <magic_enum/magic_enum.hpp>

#include "vanadium/ast/ASTNodes.h"

// NOLINTBEGIN(readability-identifier-naming)
template <>
struct magic_enum::customize::enum_range<vanadium::ast::TokenKind> {
  static constexpr int min = 0;
  static constexpr int max = std::to_underlying(vanadium::ast::TokenKind::kTokenKindEnd);
};
// NOLINTEND(readability-identifier-naming)
