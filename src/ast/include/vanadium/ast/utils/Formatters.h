#pragma once

#include <format>
#include <string_view>

#include "vanadium/ast/ASTNodes.h"

// NOLINTBEGIN(readability-convert-member-functions-to-static)

template <>
struct std::formatter<vanadium::ast::TokenKind> {
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  auto format(const vanadium::ast::TokenKind&, std::format_context&) const -> format_context::iterator;
};

template <>
struct std::formatter<vanadium::ast::NodeKind> {
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  auto format(const vanadium::ast::NodeKind&, std::format_context&) const -> format_context::iterator;
};

// NOLINTEND(readability-convert-member-functions-to-static)

namespace vanadium::ast::utils {
std::string_view TokenKindToString(TokenKind);
}
