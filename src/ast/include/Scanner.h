#pragma once

#include <string_view>
#include <vector>

#include "ASTNodes.h"

namespace vanadium::ast {
namespace parser {

class Scanner {
 public:
  Scanner(std::string_view src, ast::pos_t start_pos = 0);

  Token Scan();
  [[nodiscard]] ast::pos_t Pos() const {
    return pos_;
  }

  std::vector<pos_t>&& ExtractLineMapping() noexcept;

 private:
  void ScanWhitespace();
  void ScanLine();

  void ScanAlnum();
  void ScanDigits();

  TokenKind ScanNumber();
  TokenKind ScanMultilineComment();

  TokenKind ScanString();
  TokenKind ScanSpecialString();

  [[nodiscard]] bool HasNext(ast::pos_t extent = 0) const;
  [[nodiscard]] char Peek(ast::pos_t offset = 0) const;

  std::string_view src_;
  ast::pos_t pos_;
  std::vector<ast::pos_t> lines_;
};

}  // namespace parser
}  // namespace vanadium::ast
