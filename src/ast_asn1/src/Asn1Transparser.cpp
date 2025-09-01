#include "Asn1Transparser.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <format>
#include <iomanip>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>

#include "AST.h"
#include "ASTNodes.h"
#include "ASTTypes.h"
#include "Asn1AST.h"
#include "Asn1Scanner.h"
#include "magic_enum/magic_enum.hpp"

// TODO: hardening (missing kEOF checks, etc.)

namespace vanadium::asn1::ast {
namespace parser {

namespace {
struct ParsingCancellationSignal {};

constexpr std::string_view kVerPrefix{"ver"};
constexpr ttcn_ast::pos_t kVerPrefixSize{kVerPrefix.size()};
std::pair<ttcn_ast::pos_t, ttcn_ast::pos_t> CalcVerSequenceRange(std::uint32_t N) {
  ttcn_ast::pos_t pos = 0;

  std::uint32_t power = 1;
  std::uint32_t digits = 1;
  while ((power * 10) <= N) {
    pos += (9 * power) * (kVerPrefixSize + digits);
    power *= 10;
    digits++;
  }
  pos += (N - power) * (kVerPrefixSize + digits);
  pos += 1;

  pos -= kVerPrefixSize + 1;  // "ver1" is not needed

  return std::make_pair(pos, kVerPrefixSize + digits);
};
}  // namespace

Transparser::Transparser(lib::Arena& arena, std::string_view src) : scanner_(src), src_(src), arena_(&arena) {}

std::pair<ttcn_ast::RootNode*, std::string_view> Transparser::ParseRoot() {
  try {
    auto* root = ParseRootInternal();

    const auto source_len = src_.length();
    auto new_src = arena_->AllocStringBuffer(source_len + 1 + CalcVerSequenceRange(max_ver_ + 1).first);
    new_src[source_len] = 0;
    std::ranges::copy(src_, new_src.begin());

    auto insertion_point = new_src.begin() + source_len + 1;
    for (std::uint32_t i = 2; i <= max_ver_; i++) {
      std::ranges::copy(kVerPrefix, insertion_point);
      insertion_point += kVerPrefixSize;

      const auto result = std::to_chars(insertion_point.base(), new_src.end().base(), i);
      const auto length = result.ptr - insertion_point.base();
      insertion_point += length;
    }

    return {root, std::string_view{new_src}};
  } catch (const ParsingCancellationSignal&) {
    return {NewNode<ttcn_ast::RootNode>([&](auto&) {}), src_};
  }
}

ttcn_ast::RootNode* Transparser::ParseRootInternal() {
  return NewNode<ttcn_ast::RootNode>([&](auto& root) {
    tok_ = Peek(1).kind;

    while (tok_ != TokenKind::kEOF && tok_ != TokenKind::IDENT) {
      Consume();
    }
    if (tok_ == TokenKind::kEOF) {
      return;
    }

    auto* mod = ParseModule();
    mod->parent = &root;
    root.nodes.push_back(mod);

    if (tok_ != TokenKind::kEOF) {
      EmitError(Peek(1).range, std::format("unexpected '{}' token", magic_enum::enum_name(tok_)));
    }
  });
}

std::vector<ttcn_ast::SyntaxError>&& Transparser::GetErrors() noexcept {
  return std::move(errors_);
}

std::vector<ttcn_ast::pos_t>&& Transparser::ExtractLineMapping() noexcept {
  return scanner_.ExtractLineMapping();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Transparser::RewriteIdentName(const Token& tok) {
  auto v = tok.range.String(src_);
  auto* const p = const_cast<char*>(v.data());
  auto mut_v = std::span<char>{p, p + v.size()};

  switch (tok.kind) {
    case TokenKind::INTEGER:
      std::ranges::copy(std::string_view{"integer"}, mut_v.begin());
      break;
    case TokenKind::BOOLEAN:
      std::ranges::copy(std::string_view{"boolean"}, mut_v.begin());
      break;
    case TokenKind::STRING:
      std::ranges::copy(std::string_view{"string"}, mut_v.begin());
      break;
    default:
      std::ranges::replace(mut_v, '-', '_');
      break;
  }
}

ttcn_ast::nodes::Ident* Transparser::ParseName() {
  return NewNode<ttcn_ast::nodes::Ident>([&](auto&) {
    RewriteIdentName(Consume());
  });
}

void Transparser::ParseName(std::optional<ttcn_ast::nodes::Ident>& ident) {
  ident.emplace();
  ident->parent = last_node_;
  ident->nrange =
      [&] {
        auto tok = Consume();
        RewriteIdentName(tok);
        return tok;
      }()
          .range;
}

ttcn_ast::nodes::Module* Transparser::ParseModule() {
  return NewNode<ttcn_ast::nodes::Module>([&](ttcn_ast::nodes::Module& m) {
    tok_ = Peek(1).kind;

    ParseName(m.name);

    while (tok_ != TokenKind::kEOF && tok_ != TokenKind::BEGIN) {
      Consume();
    }
    if (tok_ == TokenKind::kEOF) {
      EmitErrorExpected("module definition");
      return;
    }
    ConsumeInvariant(TokenKind::BEGIN);

    if (tok_ == TokenKind::IMPORTS) {
      while (tok_ != TokenKind::kEOF && tok_ != TokenKind::SEMICOLON) {
        Consume();
      }
      ConsumeInvariant(TokenKind::SEMICOLON);
    }

    while (tok_ != TokenKind::END && tok_ != TokenKind::kEOF) {
      if (tok_ != TokenKind::IDENT) {
        EmitErrorExpected("definition");
        break;
      }
      auto* r = ParseDefinition();
      m.defs.emplace_back(r);
    }
    Expect(TokenKind::END);
  });
}

// REQUIREMENT: tok_ == IDENT
ttcn_ast::nodes::Definition* Transparser::ParseDefinition() {
  return NewNode<ttcn_ast::nodes::Definition>([&](ttcn_ast::nodes::Definition& d) {
    auto name = [&] {
      std::optional<ttcn_ast::nodes::Ident> opt;
      ParseName(opt);
      return *opt;
    }();

    if (tok_ == TokenKind::ASSIGN) {
      Consume();
      d.def = [&] -> ttcn_ast::Node* {
        const auto parse_struct_decl = [&](ttcn_ast::TokenKind kind) {
          return NewNode<ttcn_ast::nodes::StructTypeDecl>([&](ttcn_ast::nodes::StructTypeDecl& st) {
            Consume();
            st.kind = {.kind = kind, .range = {}};
            std::construct_at(&st.name, std::move(name));  // todo: dirty
            Expect(TokenKind::LBRACE);
            ParseFields(st.fields);
            Expect(TokenKind::RBRACE);
          });
        };
        switch (tok_) {
          case TokenKind::SEQUENCE:
            if (Peek(2).kind == TokenKind::LPAREN) {
              return NewNode<ttcn_ast::nodes::SubTypeDecl>([&](ttcn_ast::nodes::SubTypeDecl& st) {
                st.field = NewNode<ttcn_ast::nodes::Field>([&](ttcn_ast::nodes::Field& field) {
                  std::construct_at(&field.name, std::move(name));  // todo: dirty
                  field.type = ParseTypeSpec();
                });
              });
            }
            return parse_struct_decl(ttcn_ast::TokenKind::RECORD);
          case TokenKind::CHOICE:
            return parse_struct_decl(ttcn_ast::TokenKind::UNION);
          case TokenKind::ENUMERATED:
            return NewNode<ttcn_ast::nodes::EnumTypeDecl>([&](ttcn_ast::nodes::EnumTypeDecl& et) {
              Consume();
              std::construct_at(&et.name, std::move(name));  // todo: dirty
              Expect(TokenKind::LBRACE);
              ParseEnumValues(et.values);
              Expect(TokenKind::RBRACE);
            });
          default:
            return NewNode<ttcn_ast::nodes::SubTypeDecl>([&](ttcn_ast::nodes::SubTypeDecl& st) {
              st.field = NewNode<ttcn_ast::nodes::Field>([&](ttcn_ast::nodes::Field& field) {
                std::construct_at(&field.name, std::move(name));  // todo: dirty
                field.type = ParseTypeSpec();
                if (tok_ == TokenKind::LPAREN) {
                  ParseValueConstraint();
                }
              });
            });
        };
      }();
    } else {
      d.def = ParseConstDecl(std::move(name));
    }
  });
}

ttcn_ast::nodes::ValueDecl* Transparser::ParseConstDecl(ttcn_ast::nodes::Ident&& name) {
  return NewNode<ttcn_ast::nodes::ValueDecl>([&](ttcn_ast::nodes::ValueDecl& m) {
    m.kind = arena_->Alloc<ttcn_ast::Token>();
    m.kind->kind = ttcn_ast::TokenKind::CONST;

    m.type = ParseName();

    Expect(TokenKind::ASSIGN);

    m.decls.emplace_back(NewNode<ttcn_ast::nodes::Declarator>([&](ttcn_ast::nodes::Declarator& d) {
      std::construct_at(&d.name, std::move(name));  // todo: dirty
      d.value = NewNode<ttcn_ast::nodes::ValueLiteral>([&](auto& vl) {
        auto vtok = Consume();
        vl.tok = ttcn_ast::Token{
            .kind = [&] -> ttcn_ast::TokenKind {
              switch (vtok.kind) {
                case TokenKind::V_STRING:
                  return ttcn_ast::TokenKind::CHARSTRING;
                case TokenKind::V_BITSTRING:
                  return ttcn_ast::TokenKind::BITSTRING;
                case TokenKind::V_OCTETSTRING:
                  return ttcn_ast::TokenKind::OCTETSTRING;
                case TokenKind::V_HEXSTRING:
                  return ttcn_ast::TokenKind::HEXSTRING;
                case TokenKind::V_INT:
                  return ttcn_ast::TokenKind::INT;
                case TokenKind::V_FLOAT:
                  return ttcn_ast::TokenKind::FLOAT;
                case TokenKind::V_FALSE:
                  return ttcn_ast::TokenKind::FALSE;
                case TokenKind::V_TRUE:
                  return ttcn_ast::TokenKind::TRUE;
                default:
                  return ttcn_ast::TokenKind::ILLEGAL;
              }
            }(),
            .range = vtok.range,
        };
      });
    }));
  });
}

ttcn_ast::nodes::TypeSpec* Transparser::ParseTypeSpec() {
  const auto parse_ref_spec = [&](auto parse_constraint) -> ttcn_ast::nodes::RefSpec* {
    return NewNode<ttcn_ast::nodes::RefSpec>([&](auto& rs) {
      rs.x = ParseName();

      // TODO: should moved out of here
      if (tok_ == TokenKind::LPAREN) {
        Consume();
        parse_constraint();
        Expect(TokenKind::RPAREN);
      }

      if (tok_ == TokenKind::DEFAULT) {
        Consume();
        Consume();
      }
    });
  };

  const auto parse_struct_spec = [&](ttcn_ast::TokenKind kind) -> ttcn_ast::nodes::StructSpec* {
    return NewNode<ttcn_ast::nodes::StructSpec>([&](ttcn_ast::nodes::StructSpec& ss) {
      Expect(TokenKind::LBRACE);
      ss.kind = {.kind = kind, .range = {}};
      ParseFields(ss.fields);
      Expect(TokenKind::RBRACE);
    });
  };

  switch (tok_) {
    case TokenKind::BIT:
    case TokenKind::OCTET: {
      // NAME ::= BIT STRING
      //       to
      // NAME ::=  bitSTRING
      const auto specrange = Consume().range;

      if (tok_ != TokenKind::STRING) [[unlikely]] {
        EmitErrorExpected(magic_enum::enum_name(TokenKind::STRING));
      }
      auto* const rs = parse_ref_spec([&] {
        ParseSizeConstraint();
      });
      const auto& namerange = rs->x->nrange;

      // TODO: consider what to do with the necessity of ASN1 source text mutation
      auto* const mut_src = const_cast<char*>(src_.data());
      for (std::size_t i = 0; i < specrange.Length(); ++i) {
        mut_src[namerange.begin - 1 - i] = std::tolower(mut_src[specrange.end - i - 1]);
      }
      for (std::size_t i = 0; i < namerange.begin - specrange.begin - specrange.Length(); ++i) {
        mut_src[specrange.begin + i] = ' ';
      }

      rs->x->nrange.begin -= specrange.Length();
      rs->nrange.begin -= specrange.Length();

      return rs;
    }
    case TokenKind::STRING:
      return parse_ref_spec([&] {
        ParseSizeConstraint();
      });
    case TokenKind::BOOLEAN:
    case TokenKind::INTEGER:
    case TokenKind::REAL:
    case TokenKind::kNULL:
    case TokenKind::IDENT: {
      return parse_ref_spec([&] {
        ParseValueConstraint();
      });
    }

    case TokenKind::SEQUENCE: {
      Consume();
      if (tok_ == TokenKind::LPAREN) {
        Consume();
        ParseSizeConstraint();
        Expect(TokenKind::RPAREN);

        Expect(TokenKind::OF);

        return NewNode<ttcn_ast::nodes::ListSpec>([&](ttcn_ast::nodes::ListSpec& ls) {
          ls.elemtype = ParseTypeSpec();
        });
      }

      return parse_struct_spec(ttcn_ast::TokenKind::RECORD);
    }
    case TokenKind::CHOICE: {
      Consume();
      return parse_struct_spec(ttcn_ast::TokenKind::UNION);
    }

    case TokenKind::ENUMERATED: {
      Consume();
      return NewNode<ttcn_ast::nodes::EnumSpec>([&](ttcn_ast::nodes::EnumSpec& es) {
        Expect(TokenKind::LBRACE);
        ParseEnumValues(es.values);
        Expect(TokenKind::RBRACE);
      });
    }

    default: {
      EmitError(Peek(1).range, std::format("unexpected type spec '{}'", magic_enum::enum_name(tok_)));
      return nullptr;
    }
  }
}

void Transparser::ParseFields(std::vector<ttcn_ast::nodes::Field*>& fields, TokenKind term) {
  std::size_t ver{1};
  while (tok_ != term) {
    if (tok_ == TokenKind::ELIPSIS) {
      Consume();
    } else if (tok_ == TokenKind::LDBRACK) {
      Consume();
      fields.emplace_back(NewNode<ttcn_ast::nodes::Field>([&](ttcn_ast::nodes::Field& field) {
        field.name.emplace(VerIdent(++ver));
        field.type = NewNode<ttcn_ast::nodes::StructSpec>([&](ttcn_ast::nodes::StructSpec& ss) {
          ss.kind = {.kind = ttcn_ast::TokenKind::RECORD, .range = {}};
          ParseFields(ss.fields, TokenKind::RDBRACK);
        });
      }));
      Expect(TokenKind::RDBRACK);
    } else {
      fields.emplace_back(ParseField());
    }
    if (tok_ == TokenKind::COMMA) {
      Consume();
    }
  }
}

ttcn_ast::nodes::Field* Transparser::ParseField() {
  return NewNode<ttcn_ast::nodes::Field>([&](ttcn_ast::nodes::Field& f) {
    ParseName(f.name);

    f.type = ParseTypeSpec();

    if (tok_ == TokenKind::OPTIONAL) {
      Consume();
      f.optional = true;
    }
  });
}

void Transparser::ParseEnumValues(std::vector<ttcn_ast::nodes::Expr*>& values) {
  while (tok_ != TokenKind::RBRACE) {
    if (tok_ == TokenKind::ELIPSIS) {
      Consume();
    } else {
      values.emplace_back([&] -> ttcn_ast::nodes::Expr* {
        if (Peek(2).kind == TokenKind::LPAREN) {
          return NewNode<ttcn_ast::nodes::CallExpr>([&](ttcn_ast::nodes::CallExpr& ce) {
            ce.fun = ParseName();
            ce.args = NewNode<ttcn_ast::nodes::ParenExpr>([&](ttcn_ast::nodes::ParenExpr& pe) {
              Consume();
              pe.list.emplace_back(NewNode<ttcn_ast::nodes::ValueLiteral>([&](ttcn_ast::nodes::ValueLiteral& vl) {
                vl.tok = {.kind = ttcn_ast::TokenKind::INT, .range = Expect(TokenKind::V_INT).range};
              }));
              Expect(TokenKind::RPAREN);
            });
          });
        }
        return ParseName();
      }());
    }
    if (tok_ == TokenKind::COMMA) {
      Consume();
    }
  }
}

ttcn_ast::nodes::LengthExpr* Transparser::ParseSizeConstraint() {
  const auto expect_value = [&] -> bool {
    if (tok_ == TokenKind::V_INT || tok_ == TokenKind::IDENT) {
      Consume();
      return true;
    }
    EmitErrorExpected("size constraint value");
    return false;
  };

  if (tok_ == TokenKind::CONTAINING) {
    Consume();
    Consume();
    return nullptr;
  }

  Expect(TokenKind::SIZE);
  Expect(TokenKind::LPAREN);
  if (expect_value() && tok_ == TokenKind::RANGE) {
    ConsumeInvariant(TokenKind::RANGE);
    expect_value();
  }
  Expect(TokenKind::RPAREN);

  return nullptr;  // <-- TODO
}

ttcn_ast::nodes::ParenExpr* Transparser::ParseValueConstraint() {
  const auto expect_value = [&] -> bool {
    if (tok_ == TokenKind::V_INT || tok_ == TokenKind::IDENT) {
      Consume();
      return true;
    }
    EmitErrorExpected("value constraint value");
    return false;
  };

  if (expect_value() && tok_ == TokenKind::RANGE) {
    ConsumeInvariant(TokenKind::RANGE);
    expect_value();
  }

  return nullptr;  // <-- TODO
}

ttcn_ast::nodes::Ident Transparser::VerIdent(std::uint32_t ver) {
  max_ver_ = std::max(max_ver_, ver);

  const auto [pos, len] = CalcVerSequenceRange(ver);
  const ttcn_ast::pos_t begin = src_.size() + pos;

  ttcn_ast::nodes::Ident ident;
  ident.parent = last_node_;
  ident.nrange = {.begin = begin, .end = begin + len};

  return ident;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Token Transparser::Consume() {
  auto tok = queue_[cursor_];
  ++cursor_;

  Peek(1);
  tok_ = queue_[cursor_].kind;

  last_consumed_pos_ = tok.range.end;

  return tok;
}

Token Transparser::ConsumeInvariant(TokenKind) {
  return Consume();
}

void Transparser::Grow(std::uint8_t n) {
  constexpr auto kShouldIgnore = [](TokenKind kind) -> bool {
    switch (kind) {
      case TokenKind::COMMENT:
        return true;
      default:
        return false;
    }
  };

  while (n > 0) {
    auto token = scanner_.Scan();
    if (kShouldIgnore(token.kind)) {
      continue;
    }

    if (token.kind == TokenKind::MALFORMED || token.kind == TokenKind::UNTERMINATED) {
      EmitError(token.range, "malformed or unterminated token");
      continue;
    }
    queue_.push_back(token);
    --n;
  }
}

Token& Transparser::Peek(std::uint8_t i) {
  const auto idx = cursor_ + i - 1;
  const auto cached = queue_.size();
  if (idx + 1 > cached) {
    Grow(idx - cached + 1);
  }
  return queue_[idx];
}

Token Transparser::Expect(TokenKind expected) {
  if (tok_ != expected) [[unlikely]] {
    EmitErrorExpected(magic_enum::enum_name(expected));
    return {};
  }
  return Consume();
}

void Transparser::EmitError(const ttcn_ast::Range& range, std::string&& message) {
  errors_.emplace_back(range, std::move(message));
  throw ParsingCancellationSignal{};
}
void Transparser::EmitErrorExpected(std::string_view what) {
  errors_.emplace_back(Peek(1).range, std::format("expected {}", what));
  throw ParsingCancellationSignal{};
}

//

Token* Transparser::TokAlloc(Token&& token) {
  return arena_->Alloc<Token>(std::move(token));
}

// use explicit type in lambda instead of auto for clangd to provide assistance
template <ttcn_ast::IsNode T, typename Initializer>
  requires std::is_invocable_v<Initializer, T&>
T* Transparser::NewNode(Initializer f) {
  auto* p = arena_->Alloc<T>();
  p->nrange.begin = Peek(1).range.begin;
  //
  auto* top = last_node_;
  p->parent = top;
  last_node_ = p;
  //
  if constexpr (std::is_same_v<typename std::invoke_result_t<Initializer, T&>, bool>) {
    if (!f(*p)) {
      return nullptr;
    }
  } else {
    f(*p);
  }
  //
  p->nrange.end = last_consumed_pos_;
  //
  last_node_ = top;
  return p;
}

}  // namespace parser
}  // namespace vanadium::asn1::ast
