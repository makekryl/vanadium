#pragma once

#include <string_view>
#include <vector>

#include "vanadium/asn1/ast/Asn1ASTNodes.h"
#include "vanadium/asn1/ast/Asn1ASTTypes.h"

namespace vanadium::asn1::ast {
namespace parser {

class Scanner {
 public:
  Scanner(std::string_view src, pos_t start_pos = 0);

  Token Scan();
  [[nodiscard]] pos_t Pos() const {
    return pos_;
  }

  std::vector<pos_t>&& ExtractLineMapping() noexcept;

 private:
  void ScanWhitespace();
  void ScanLine();

  void ScanAlnum();
  void ScanDigits();

  TokenKind ScanComment();

  TokenKind ScanNumber();

  TokenKind ScanString();
  TokenKind ScanSpecialString();

  [[nodiscard]] bool HasNext(pos_t extent = 0) const;
  [[nodiscard]] char Peek(pos_t offset = 0) const;

  std::string_view src_;
  pos_t pos_;
  std::vector<pos_t> lines_;
};

}  // namespace parser
}  // namespace vanadium::asn1::ast
