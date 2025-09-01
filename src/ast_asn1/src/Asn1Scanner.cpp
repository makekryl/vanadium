#include "Asn1Scanner.h"

#include <cctype>
#include <string_view>

#include "Asn1AST.h"
#include "StaticMap.h"

namespace vanadium::asn1::ast {
namespace parser {

constexpr auto kKeywordLookup = lib::MakeStaticMap<std::string_view, TokenKind>({
    {"DEFINITIONS", TokenKind::DEFINITIONS},
    {"AUTOMATIC", TokenKind::AUTOMATIC},
    {"TAGS", TokenKind::TAGS},
    {"IMPORTS", TokenKind::IMPORTS},

    {"BEGIN", TokenKind::BEGIN},
    {"END", TokenKind::END},

    {"SEQUENCE", TokenKind::SEQUENCE},
    {"CHOICE", TokenKind::CHOICE},
    {"ENUMERATED", TokenKind::ENUMERATED},

    {"INTEGER", TokenKind::INTEGER},
    {"REAL", TokenKind::REAL},
    {"STRING", TokenKind::STRING},
    {"BIT", TokenKind::BIT},
    {"OCTET", TokenKind::OCTET},
    {"BOOLEAN", TokenKind::BOOLEAN},

    {"DEFAULT", TokenKind::DEFAULT},
    {"OPTIONAL", TokenKind::OPTIONAL},
    {"SIZE", TokenKind::SIZE},
    {"CONTAINING", TokenKind::CONTAINING},

    {"OF", TokenKind::OF},

    {"NULL", TokenKind::kNULL},
});

Scanner::Scanner(std::string_view src, ttcn_ast::pos_t start_pos) : src_(src), pos_(start_pos), lines_({0}) {}

Token Scanner::Scan() {
  ScanWhitespace();

  if (!HasNext()) [[unlikely]] {
    return {
        .kind = TokenKind::kEOF,
        .range = {.begin = pos_, .end = pos_ + 1},
    };
  }

  const auto start_pos = pos_;
  const char ch = Peek();
  //
  ++pos_;
  const char next = HasNext() ? Peek() : 0;

  TokenKind kind{TokenKind::ILLEGAL};

  if (std::isalpha(ch)) {
    kind = TokenKind::IDENT;
    ScanAlnum();
    if (pos_ - start_pos > 1) {
      // map to keyword if it is one
      if (const auto it = kKeywordLookup.get(src_.substr(start_pos, pos_ - start_pos)); it) {
        kind = *it;
      }
    }
  } else if (std::isdigit(ch)) {
    kind = ScanNumber();
  } else {
    switch (ch) {
      case ',':
        kind = TokenKind::COMMA;
        break;
      case ';':
        kind = TokenKind::SEMICOLON;
        break;
      case '(':
        kind = TokenKind::LPAREN;
        break;
      case ')':
        kind = TokenKind::RPAREN;
        break;
      case '{':
        kind = TokenKind::LBRACE;
        break;
      case '}':
        kind = TokenKind::RBRACE;
        break;
      case '[':
        if (next == '[') {
          kind = TokenKind::LDBRACK;
          ++pos_;
        }
        break;
      case ']':
        if (next == ']') {
          kind = TokenKind::RDBRACK;
          ++pos_;
        }
        break;
      case '-':
        if (next == '-') {
          kind = TokenKind::COMMENT;
          ++pos_;
          ScanLine();
        } else if (std::isdigit(next)) {
          kind = ScanNumber();
        }
        break;
      case '.':
        if (next == '.') {
          ++pos_;
          kind = TokenKind::RANGE;
          if (HasNext() && Peek() == '.') {
            ++pos_;
            kind = TokenKind::ELIPSIS;
          }
        }
        break;
      case ':':
        if (next == ':') {
          ++pos_;
          if (HasNext() && Peek() == '=') {
            ++pos_;
            kind = TokenKind::ASSIGN;
          }
        }
        break;
      case '\'':
        kind = ScanSpecialString();
        break;
      case '"':
        kind = ScanString();
        break;
      default:
        kind = TokenKind::ILLEGAL;
        break;
    }
  }

  return {
      .kind = kind,
      .range = {.begin = start_pos, .end = pos_},
  };
}

void Scanner::ScanWhitespace() {
  while (HasNext()) {
    switch (Peek()) {
      case ' ':
      case '\t':
      case '\r':
        break;
      case '\n':
      case '\v':
      case '\f':
        lines_.push_back(pos_ + 1);
        break;
      default:
        return;
    }
    ++pos_;
  }
}

void Scanner::ScanLine() {
  while (HasNext() && Peek() != '\n') {
    ++pos_;
  }
}

void Scanner::ScanAlnum() {
  const auto is_alnum_ex = [](char c) -> bool {
    return std::isalnum(c) || c == '-';
  };
  while (HasNext() && is_alnum_ex(Peek())) {
    ++pos_;
  }
}

void Scanner::ScanDigits() {
  while (HasNext() && std::isdigit(Peek())) {
    ++pos_;
  }
}

TokenKind Scanner::ScanNumber() {
  TokenKind kind = TokenKind::V_INT;

  if (src_[pos_ - 1] != '0') {
    ScanDigits();
  }

  // scan fractional part
  if (HasNext() && Peek() == '.') {
    if (HasNext(1) && Peek(1) == '.') {
      return kind;
    }
    kind = TokenKind::V_FLOAT;
    ++pos_;
    ScanDigits();
  }

  // scan exponent
  if (HasNext() && (Peek() == 'e' || Peek() == 'E')) {
    kind = TokenKind::V_FLOAT;
    ++pos_;
    if (HasNext() && (Peek() == '+' || Peek() == '-')) {
      ++pos_;
    }
    if (!HasNext() || !std::isdigit(Peek())) {
      kind = TokenKind::MALFORMED;
    } else {
      ScanDigits();
    }
  }

  // garbage
  if (HasNext() && std::isalnum(Peek())) {
    kind = TokenKind::MALFORMED;
    ScanAlnum();
  }

  return kind;
}

TokenKind Scanner::ScanString() {
  --pos_;  // backup for proper quoting ("")
  while (HasNext()) {
    ++pos_;
    switch (Peek()) {
      case '\n':
      case '\v':
      case '\f':
        lines_.push_back(pos_ + 1);
        break;
      case '\\':
        ++pos_;
        break;
      case '"':
        ++pos_;
        if (!HasNext() || Peek() != '"') {
          return TokenKind::STRING;
        }
        break;
      default:
        break;
    }
  }
  return TokenKind::UNTERMINATED;
}

TokenKind Scanner::ScanSpecialString() {
  while (true) {
    switch (Peek()) {
      case '\n':
      case '\v':
      case '\f':
        lines_.push_back(pos_ + 1);
        break;
      case '\'':
        ++pos_;
        goto exit_loop;
      default:
        ++pos_;
        if (!HasNext()) {
          return TokenKind::UNTERMINATED;
        }
    }
  }
exit_loop:

  TokenKind kind = TokenKind::MALFORMED;
  if (HasNext()) {
    switch (Peek()) {
      case 'B':
        kind = TokenKind::V_BITSTRING;
        ++pos_;
        break;
      case 'H':
        kind = TokenKind::V_HEXSTRING;
        ++pos_;
        break;
      case 'O':
        kind = TokenKind::V_OCTETSTRING;
        ++pos_;
        break;
      default:
        break;
    }
  }
  return kind;
}

inline bool Scanner::HasNext(ttcn_ast::pos_t extent) const {
  return (pos_ + extent) < src_.length();
}
inline char Scanner::Peek(ttcn_ast::pos_t offset) const {
  return src_[pos_ + offset];
}

std::vector<ttcn_ast::pos_t>&& Scanner::ExtractLineMapping() noexcept {
  return std::move(lines_);
}

}  // namespace parser
}  // namespace vanadium::asn1::ast
