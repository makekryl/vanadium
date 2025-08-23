#pragma once

#include <string_view>
#include <vector>

#include "ASTTypes.h"
#include "Asn1AST.h"

namespace vanadium::asn1::ast {
namespace parser {

class Scanner {
 public:
  Scanner(std::string_view src, ttcn_ast::pos_t start_pos = 0);

  Token Scan();
  [[nodiscard]] ttcn_ast::pos_t Pos() const {
    return pos_;
  }

  std::vector<ttcn_ast::pos_t>&& ExtractLineMapping() noexcept;

 private:
  void ScanWhitespace();
  void ScanLine();

  void ScanAlnum();
  void ScanDigits();

  TokenKind ScanNumber();

  TokenKind ScanString();
  TokenKind ScanSpecialString();

  [[nodiscard]] bool HasNext(ttcn_ast::pos_t extent = 0) const;
  [[nodiscard]] char Peek(ttcn_ast::pos_t offset = 0) const;

  std::string_view src_;
  ttcn_ast::pos_t pos_;
  std::vector<ttcn_ast::pos_t> lines_;
};

}  // namespace parser
}  // namespace vanadium::asn1::ast
