#pragma once

#include <cstdint>

#include "AST.h"
#include "ASTTypes.h"

namespace vanadium::ls {

struct LSPosition {
  core::ast::pos_t line;
  std::uint32_t character;
};

inline LSPosition ToLSPosition(const core::ast::AST& ast, core::ast::pos_t pos) {
  const core::ast::pos_t line = ast.lines.LineOf(pos);

  std::uint32_t character{0};
  // for (char c : ast.src.substr(ast.lines.StartOf(line), pos)) {
  //   if (c >= 0xd800 && c <= 0xdbff)
  //     string_pos += 2;
  //   else
  //     ++string_pos;
  // }

  return {
      .line = line,
      .character = character,
  };
}

}  // namespace vanadium::ls
