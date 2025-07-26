#include "Scanner.h"

#include <cctype>
#include <string_view>

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "StaticMap.h"

namespace vanadium::core::ast {
namespace parser {

constexpr auto kKeywordLookup = lib::MakeStaticMap<std::string_view, TokenKind>({
    {"mod", TokenKind::MOD},
    {"rem", TokenKind::REM},

    {"and", TokenKind::AND},
    {"or", TokenKind::OR},
    {"xor", TokenKind::XOR},
    {"not", TokenKind::NOT},

    {"and4b", TokenKind::AND4B},
    {"or4b", TokenKind::OR4B},
    {"xor4b", TokenKind::XOR4B},
    {"not4b", TokenKind::NOT4B},

    {"address", TokenKind::ADDRESS},
    {"alive", TokenKind::ALIVE},
    {"all", TokenKind::ALL},
    {"alt", TokenKind::ALT},
    {"altstep", TokenKind::ALTSTEP},
    {"any", TokenKind::ANYKW},
    {"break", TokenKind::BREAK},
    {"case", TokenKind::CASE},
    {"charstring", TokenKind::CHARSTRING},
    {"class", TokenKind::CLASS},
    {"component", TokenKind::COMPONENT},
    {"const", TokenKind::CONST},
    {"continue", TokenKind::CONTINUE},
    {"control", TokenKind::CONTROL},
    {"create", TokenKind::CREATE},
    {"decmatch", TokenKind::DECMATCH},
    {"display", TokenKind::DISPLAY},
    {"do", TokenKind::DO},
    {"else", TokenKind::ELSE},
    {"encode", TokenKind::ENCODE},
    {"enumerated", TokenKind::ENUMERATED},
    {"error", TokenKind::ERROR},
    {"except", TokenKind::EXCEPT},
    {"exception", TokenKind::EXCEPTION},
    {"extends", TokenKind::EXTENDS},
    {"extension", TokenKind::EXTENSION},
    {"external", TokenKind::EXTERNAL},
    {"fail", TokenKind::FAIL},
    {"false", TokenKind::FALSE},
    {"for", TokenKind::FOR},
    {"friend", TokenKind::FRIEND},
    {"from", TokenKind::FROM},
    {"function", TokenKind::FUNCTION},
    {"goto", TokenKind::GOTO},
    {"group", TokenKind::GROUP},
    {"if", TokenKind::IF},
    {"ifpresent", TokenKind::IFPRESENT},
    {"import", TokenKind::IMPORT},
    {"in", TokenKind::IN},
    {"inconc", TokenKind::INCONC},
    {"inout", TokenKind::INOUT},
    {"interleave", TokenKind::INTERLEAVE},
    {"label", TokenKind::LABEL},
    {"language", TokenKind::LANGUAGE},
    {"length", TokenKind::LENGTH},
    {"map", TokenKind::MAP},
    {"message", TokenKind::MESSAGE},
    {"mixed", TokenKind::MIXED},
    {"modifies", TokenKind::MODIFIES},
    {"module", TokenKind::MODULE},
    {"modulepar", TokenKind::MODULEPAR},
    {"mtc", TokenKind::MTC},
    {"not_a_number", TokenKind::kNaN},
    {"noblock", TokenKind::NOBLOCK},
    {"none", TokenKind::NONE},
    {"null", TokenKind::kNULL},
    {"of", TokenKind::OF},
    {"omit", TokenKind::OMIT},
    {"on", TokenKind::ON},
    {"optional", TokenKind::OPTIONAL},
    {"out", TokenKind::OUT},
    {"override", TokenKind::OVERRIDE},
    {"param", TokenKind::PARAM},
    {"pass", TokenKind::PASS},
    {"pattern", TokenKind::PATTERN},
    {"port", TokenKind::PORT},
    {"present", TokenKind::PRESENT},
    {"private", TokenKind::PRIVATE},
    {"procedure", TokenKind::PROCEDURE},
    {"public", TokenKind::PUBLIC},
    {"realtime", TokenKind::REALTIME},
    {"record", TokenKind::RECORD},
    {"regexp", TokenKind::REGEXP},
    {"repeat", TokenKind::REPEAT},
    {"return", TokenKind::RETURN},
    {"runs", TokenKind::RUNS},
    {"select", TokenKind::SELECT},
    {"sender", TokenKind::SENDER},
    {"set", TokenKind::SET},
    {"signature", TokenKind::SIGNATURE},
    {"stepsize", TokenKind::STEPSIZE},
    {"system", TokenKind::SYSTEM},
    {"template", TokenKind::TEMPLATE},
    {"testcase", TokenKind::TESTCASE},
    {"timer", TokenKind::TIMER},
    {"timestamp", TokenKind::TIMESTAMP},
    {"to", TokenKind::TO},
    {"true", TokenKind::TRUE},
    {"type", TokenKind::TYPE},
    {"union", TokenKind::UNION},
    {"universal", TokenKind::UNIVERSAL},
    {"unmap", TokenKind::UNMAP},
    {"value", TokenKind::VALUE},
    {"var", TokenKind::VAR},
    {"variant", TokenKind::VARIANT},
    {"while", TokenKind::WHILE},
    {"with", TokenKind::WITH},
});

Scanner::Scanner(std::string_view src, pos_t start_pos) : src_(src), pos_(start_pos), lines_({0}) {}

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

  if (std::isalpha(ch) || ch == '_') {
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
      case '+':
        kind = TokenKind::ADD;
        if (next == '+') {
          kind = TokenKind::INC;
          ++pos_;
        }
        break;
      case '*':
        kind = TokenKind::MUL;
        break;
      case '&':
        kind = TokenKind::CONCAT;
        break;
      case '?':
        kind = TokenKind::ANY;
        break;
      case '(':
        kind = TokenKind::LPAREN;
        break;
      case '[':
        kind = TokenKind::LBRACK;
        break;
      case '{':
        kind = TokenKind::LBRACE;
        break;
      case ')':
        kind = TokenKind::RPAREN;
        break;
      case ']':
        kind = TokenKind::RBRACK;
        break;
      case '}':
        kind = TokenKind::RBRACE;
        break;
      case ';':
        kind = TokenKind::SEMICOLON;
        break;
      case '/':
        switch (next) {
          case '/':
            ScanLine();
            kind = TokenKind::COMMENT;
            break;
          case '*':
            kind = ScanMultilineComment();
            break;
          default:
            kind = TokenKind::DIV;
            break;
        }
        break;
      case '@':
        if (std::isalpha(next)) {
          ScanAlnum();
          kind = TokenKind::MODIF;
        } else if (next == '>') {
          ++pos_;
          kind = TokenKind::ROR;
        } else {
          kind = TokenKind::ILLEGAL;
        }
        break;
      case '%':
        if (std::isalpha(next)) {
          ScanAlnum();
          kind = TokenKind::IDENT;
        }
        break;
      case '!':
        kind = TokenKind::EXCL;
        if (next == '=') {
          ++pos_;
          kind = TokenKind::NE;
        }
        break;
      case '-':
        switch (next) {
          case '>':
            ++pos_;
            kind = TokenKind::REDIR;
            break;
          case '-':
            ++pos_;
            kind = TokenKind::DEC;
            break;
          default:
            kind = TokenKind::SUB;
            break;
        }
        break;
      case '.':
        kind = TokenKind::DOT;
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
        switch (next) {
          case ':':
            ++pos_;
            kind = TokenKind::COLONCOLON;
            break;
          case '=':
            ++pos_;
            kind = TokenKind::ASSIGN;
            break;
          default:
            kind = TokenKind::COLON;
            break;
        }
        break;
      case '<':
        switch (next) {
          case '<':
            ++pos_;
            kind = TokenKind::SHL;
            break;
          case '=':
            ++pos_;
            kind = TokenKind::LE;
            break;
          case '@':
            ++pos_;
            kind = TokenKind::ROL;
            break;
          default:
            kind = TokenKind::LT;
            break;
        }
        break;
      case '=':
        switch (next) {
          case '=':
            ++pos_;
            kind = TokenKind::EQ;
            break;
          case '>':
            ++pos_;
            kind = TokenKind::DECODE;
            break;
          default:
            break;
        }
        break;
      case '>':
        switch (next) {
          case '>':
            ++pos_;
            kind = TokenKind::SHR;
            break;
          case '=':
            ++pos_;
            kind = TokenKind::GE;
            break;
          default:
            kind = TokenKind::GT;
            break;
        }
        break;
      case '\'':
        kind = ScanSpecialString();
        break;
      case '#':
        ScanLine();
        kind = TokenKind::PREPROC;
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
    return std::isalnum(c) || c == '_';
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
  TokenKind kind = TokenKind::INT;

  if (src_[pos_ - 1] != '0') {
    ScanDigits();
  }

  // scan fractional part
  if (HasNext() && Peek() == '.') {
    if (HasNext(1) && Peek(1) == '.') {
      return kind;
    }
    kind = TokenKind::FLOAT;
    ++pos_;
    ScanDigits();
  }

  // scan exponent
  if (HasNext() && (Peek() == 'e' || Peek() == 'E')) {
    kind = TokenKind::FLOAT;
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

TokenKind Scanner::ScanMultilineComment() {
  ++pos_;  // skip the first '*'
  while (HasNext()) {
    const char ch = Peek();
    if (ch == '\n' || ch == '\v' || ch == '\f') {
      lines_.push_back(pos_ + 1);
    }
    ++pos_;
    if (ch == '*' && HasNext() && Peek() == '/') {
      ++pos_;
      return TokenKind::COMMENT;
    }
  }
  return TokenKind::UNTERMINATED;
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
        kind = TokenKind::BITSTRING;
        ++pos_;
        break;
      case 'H':
        kind = TokenKind::HEXSTRING;
        ++pos_;
        break;
      case 'O':
        kind = TokenKind::OCTETSTRING;
        ++pos_;
        break;
      default:
        break;
    }
  }
  return kind;
}

inline bool Scanner::HasNext(ast::pos_t extent) const {
  return (pos_ + extent) < src_.length();
}
inline char Scanner::Peek(ast::pos_t offset) const {
  return src_[pos_ + offset];
}

std::vector<pos_t>&& Scanner::ExtractLineMapping() noexcept {
  return std::move(lines_);
}

}  // namespace parser
}  // namespace vanadium::core::ast
