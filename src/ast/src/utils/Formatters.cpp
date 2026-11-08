#include "vanadium/ast/utils/Formatters.h"

#include <format>
#include <string_view>

#include <magic_enum/magic_enum.hpp>

#include "vanadium/ast/ASTNodes.h"
#include "vanadium/ast/defs/magic_enum_defs.h"

// NOLINTBEGIN(readability-convert-member-functions-to-static)

auto std::formatter<vanadium::ast::TokenKind>::format(const vanadium::ast::TokenKind& v, std::format_context& ctx) const
    -> std::format_context::iterator {
  return std::format_to(ctx.out(), "{}", magic_enum::enum_name(v));
}

auto std::formatter<vanadium::ast::NodeKind>::format(const vanadium::ast::NodeKind& v, std::format_context& ctx) const
    -> std::format_context::iterator {
  return std::format_to(ctx.out(), "{}", magic_enum::enum_name(v));
}

// NOLINTEND(readability-convert-member-functions-to-static)

namespace vanadium::ast::utils {
std::string_view TokenKindToString(TokenKind v) {
  return magic_enum::enum_name(v);
}
}  // namespace vanadium::ast::utils
