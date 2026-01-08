#pragma once

#include <vanadium/ast/ASTTypes.h>

#include <cstdint>

namespace vanadium::asn1 {
namespace ast {

namespace ttcn_ast = vanadium::ast;

// NOLINTBEGIN(readability-identifier-naming)
enum class TokenKind : std::uint8_t {
  ILLEGAL,

  kEOF,
  UNTERMINATED,
  MALFORMED,

  //

  COMMENT,

  //

  IDENT,          // main
  V_INT,          // 12345
  V_FLOAT,        // 123.45
  V_STRING,       // "abc"
  V_BITSTRING,    // '101?F'B
  V_HEXSTRING,    // '101?F'H
  V_OCTETSTRING,  // '101?F'O
  V_TRUE,         // true
  V_FALSE,        // false

  //

  DOT,          // .
  COMMA,        // ,
  SEMICOLON,    // ;
  VERTICALBAR,  // |

  REF,  // &

  RANGE,     // ..
  ELLIPSIS,  // ...
  ASSIGN,    // ::=

  LPAREN,   // (
  RPAREN,   // )
  LBRACE,   // {
  RBRACE,   // }
  LDBRACK,  // [[
  RDBRACK,  // ]]

  //

  DEFINITIONS,
  AUTOMATIC,
  TAGS,
  IMPORTS,
  FROM,

  BEGIN,
  END,

  SEQUENCE,
  CHOICE,
  ENUMERATED,
  CLASS,

  INTEGER,
  REAL,
  STRING,
  BIT,
  OCTET,
  BOOLEAN,

  OBJECT,
  UNIQUE,

  DEFAULT,
  OPTIONAL,
  SIZE,
  CONTAINING,
  WITH,

  OF,

  kNULL,
};
// NOLINTEND(readability-identifier-naming)

struct Token {
  TokenKind kind;
  ttcn_ast::Range range;

  [[nodiscard]] std::string_view On(std::string_view src) const noexcept {
    return range.String(src);
  }
};

}  // namespace ast
}  // namespace vanadium::asn1
