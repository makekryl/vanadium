#include "vanadium/asn1/ast/Asn1Parser.h"

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/Scanner.h>

#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <print>
#include <stacktrace>
#include <string_view>
#include <utility>

#include "StaticSet.h"
#include "vanadium/asn1/ast/Asn1AST.h"
#include "vanadium/asn1/ast/Asn1ASTNodes.h"
#include "vanadium/asn1/ast/Asn1ASTTypes.h"
#include "vanadium/asn1/ast/Asn1Scanner.h"

namespace vanadium::asn1::ast {
namespace parser {

namespace {
struct ParsingCancellationSignal {};

// clang-format off
constexpr auto kTokTopLevel = lib::MakeStaticSet<TokenKind>({
  TokenKind::IDENT,
});
// clang-format on

inline void IncorporateNode(Node& n, Node* x) {
  n.parent = std::exchange(x->parent, &n);
  n.nrange.begin = x->nrange.begin;
}
}  // namespace

Parser::Parser(lib::Arena& arena, std::string_view src) : scanner_(src), src_(src), arena_(&arena) {}

RootNode* Parser::ParseRoot() {
  try {
    return ParseRootImpl();
  } catch (const ParsingCancellationSignal&) {
    return nullptr;
  }
}

std::vector<SyntaxError>&& Parser::GetErrors() noexcept {
  return std::move(errors_);
}

std::vector<pos_t>&& Parser::ExtractLineMapping() noexcept {
  return scanner_.ExtractLineMapping();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RootNode* Parser::ParseRootImpl() {
  return NewNode<RootNode>([&](auto& root) {
    tok_ = Peek(1).kind;
    while (tok_ != TokenKind::kEOF) {
      auto* node = ParseModule();
      node->parent = &root;
      root.nodes.push_back(node);
      if (tok_ != TokenKind::kEOF && !kTokTopLevel.contains(tok_)) {
        EmitError(Peek(1).range, std::format("unexpected '{}' token", magic_enum::enum_name(tok_)));
        while (scanner_.Scan().kind != TokenKind::kEOF) {
          // eat all tokens to calculate remaining line offsets
          ;
        }
        break;
      }
    }
  });
}

static bool IsTypeStart(TokenKind t) {
  return t == TokenKind::IDENT || t == TokenKind::SEQUENCE || t == TokenKind::SET || t == TokenKind::CLASS ||
         t == TokenKind::CHOICE || t == TokenKind::INTEGER;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Node* Parser::Parse() {
  switch (tok_) {
    case TokenKind::IDENT:
      return ParseModule();
      break;
    default:
      EmitErrorExpected("idk");
      break;
  }
  return nullptr;
}

void Parser::ParseIdent(nodes::Ident& ident) {
  ident.parent = last_node_;
  ident.nrange = Expect(TokenKind::IDENT).range;
}

nodes::Ident* Parser::ParseIdent() {
  return NewNode<nodes::Ident>([&](auto&) {
    Expect(TokenKind::IDENT);
  });
}

nodes::Module* Parser::ParseModule() {
  return NewNode<nodes::Module>([&](nodes::Module& m) {
    ParseIdent(m.name);

    if (tok_ == TokenKind::LBRACE) {
      ConsumeInvariant(TokenKind::LBRACE);
      while (tok_ != TokenKind::RBRACE) {
        m.oidComponents.push_back(ParseOidComponent());
      }
      Expect(TokenKind::RBRACE);
    }

    Expect(TokenKind::DEFINITIONS);

    switch (tok_) {
      case TokenKind::EXPLICIT:
      case TokenKind::IMPLICIT:
      case TokenKind::AUTOMATIC:
        m.tagDefault = Consume();
        Expect(TokenKind::TAGS);
        break;
      default:
        m.tagDefault = {.kind = TokenKind::kSentinel};
        break;
    }

    if (tok_ == TokenKind::EXTENSIBILITY) {
      ConsumeInvariant(TokenKind::EXTENSIBILITY);
      m.extensibility = Consume();
    } else {
      m.extensibility = {.kind = TokenKind::kSentinel};
    }

    Expect(TokenKind::ASSIGN);
    Expect(TokenKind::BEGIN);

    if (tok_ == TokenKind::EXPORTS) {
      m.exports = ParseModuleExports();
    }
    if (tok_ == TokenKind::IMPORTS) {
      m.imports = ParseModuleImports();
    }

    while (tok_ != TokenKind::END) {
      m.assignments.push_back(ParseAssignment());
    }

    Expect(TokenKind::END);
  });
}

nodes::ModuleExports* Parser::ParseModuleExports() {
  return NewNode<nodes::ModuleExports>([&](nodes::ModuleExports& e) {
    Consume();
    if (tok_ == TokenKind::ALL) {
      Consume();
      e.all = true;
    } else {
      while (tok_ == TokenKind::IDENT) {
        e.symbols.push_back(ParseSymbol());
        if (tok_ != TokenKind::COMMA) break;
        Consume();
      }
    }
    Expect(TokenKind::SEMICOLON);
  });
}

nodes::ModuleImports* Parser::ParseModuleImports() {
  return NewNode<nodes::ModuleImports>([&](nodes::ModuleImports& i) {
    Consume();
    while (tok_ == TokenKind::IDENT) i.symbolsFromModules.push_back(ParseSymbolsFromModule());
    Expect(TokenKind::SEMICOLON);
  });
}

nodes::SymbolsFromModule* Parser::ParseSymbolsFromModule() {
  return NewNode<nodes::SymbolsFromModule>([&](nodes::SymbolsFromModule& s) {
    while (tok_ == TokenKind::IDENT) {
      s.symbols.push_back(ParseSymbol());
      if (tok_ != TokenKind::COMMA) break;
      Consume();
    }
    Expect(TokenKind::FROM);
    ParseIdent(s.fromModule);
  });
}

nodes::Symbol* Parser::ParseSymbol() {
  return NewNode<nodes::Symbol>([&](nodes::Symbol& s) {
    ParseIdent(s.ident);
    if (tok_ == TokenKind::LBRACE) {
      Consume();
      Expect(TokenKind::RBRACE);
    }
  });
}

// ---------- Assignments ----------

nodes::Assignment* Parser::ParseAssignment() {
  return NewNode<nodes::Assignment>([&](nodes::Assignment& a) {
    ParseIdent(a.name);

    if (tok_ == TokenKind::ASSIGN)
      a.rhs = ParseTypeAssignment();
    else if (IsTypeStart(tok_))
      a.rhs = ParseValueAssignment();
    else
      a.rhs = ParseParameterizedAssignment();
  });
}

nodes::TypeAssignment* Parser::ParseTypeAssignment() {
  return NewNode<nodes::TypeAssignment>([&](nodes::TypeAssignment& t) {
    Expect(TokenKind::ASSIGN);
    t.type = ParseAsnType();
  });
}

nodes::ValueAssignment* Parser::ParseValueAssignment() {
  return NewNode<nodes::ValueAssignment>([&](nodes::ValueAssignment& v) {
    v.type = ParseAsnType();
    Expect(TokenKind::ASSIGN);
    v.value = ParseValue();
  });
}

nodes::ParameterizedAssignment* Parser::ParseParameterizedAssignment() {
  return NewNode<nodes::ParameterizedAssignment>([&](nodes::ParameterizedAssignment& p) {
    p.parameters = ParseParameterList();
    Expect(TokenKind::ASSIGN);
    p.rhs = ParseAnyRhs();
  });
}

// ---------- Types ----------

nodes::Tag* Parser::ParseTag() {
  return NewNode<nodes::Tag>([&](nodes::Tag& t) {
    Expect(TokenKind::LBRACK);
    if (tok_ != TokenKind::V_INT) {
      switch (tok_) {
        case TokenKind::UNIVERSAL:
        case TokenKind::APPLICATION:
        case TokenKind::PRIVATE:
          t.tagClass = Consume();
          break;
        default:
          EmitErrorExpected("tag class");
          break;
      }
    }
    t.number = Expect(TokenKind::V_INT);
    Expect(TokenKind::RBRACK);

    switch (tok_) {
      case TokenKind::IMPLICIT:
      case TokenKind::EXPLICIT:
        t.mode = Consume();
        break;
      default:
        break;
    }
  });
}

nodes::AsnType* Parser::ParseAsnType() {
  return NewNode<nodes::AsnType>([&](nodes::AsnType& t) {
    if (tok_ == TokenKind::LBRACK) {
      t.tag = ParseTag();
    }
    t.base = ParseTypeBase();
    while (tok_ == TokenKind::LPAREN) t.constraints.push_back(ParseConstraint());
  });
}

Node* Parser::ParseTypeBase() {
  const auto parse_composite_type = [&](TokenKind second_tok) {
    return NewNode<nodes::BuiltinTypeIdent>([&](nodes::BuiltinTypeIdent& m) {
      m.tok1 = Consume();
      m.tok2 = Expect(second_tok);
    });
  };

  switch (tok_) {
    case TokenKind::SEQUENCE:
      return ParseSequenceType();
    case TokenKind::SET:
      return ParseSetType();
    case TokenKind::CHOICE:
      return ParseChoiceType();
    case TokenKind::ENUMERATED:
      return ParseEnumeratedType();

    case TokenKind::OBJECT:
      return parse_composite_type(TokenKind::IDENTIFIER);

    case TokenKind::BIT:
    case TokenKind::OCTET:
      return parse_composite_type(TokenKind::STRING);

    case TokenKind::INTEGER:
    case TokenKind::REAL:
    case TokenKind::BOOLEAN:
    case TokenKind::kNULL:
      return NewNode<nodes::BuiltinTypeIdent>([&](nodes::BuiltinTypeIdent& m) {
        m.tok1 = Consume();
        m.tok2 = {.kind = TokenKind::kSentinel};
      });

    default:
      return ParseDefinedType();
  }
}

nodes::SequenceType* Parser::ParseSequenceType() {
  return NewNode<nodes::SequenceType>([&](nodes::SequenceType& t) {
    Expect(TokenKind::SEQUENCE);
    Expect(TokenKind::LBRACE);

    if (tok_ != TokenKind::RBRACE) {
      t.components = ParseComponentTypeList();
    }

    Expect(TokenKind::RBRACE);
  });
}

nodes::SetType* Parser::ParseSetType() {
  return NewNode<nodes::SetType>([&](nodes::SetType& t) {
    Expect(TokenKind::SET);
    Expect(TokenKind::LBRACE);

    if (tok_ != TokenKind::RBRACE) {
      t.components = ParseComponentTypeList();
    }

    Expect(TokenKind::RBRACE);
  });
}

nodes::ChoiceType* Parser::ParseChoiceType() {
  return NewNode<nodes::ChoiceType>([&](nodes::ChoiceType& t) {
    Expect(TokenKind::CHOICE);
    Expect(TokenKind::LBRACE);

    t.alternatives = ParseNamedTypeList();

    Expect(TokenKind::RBRACE);
  });
}

nodes::EnumeratedType* Parser::ParseEnumeratedType() {
  return NewNode<nodes::EnumeratedType>([&](nodes::EnumeratedType& t) {
    Expect(TokenKind::ENUMERATED);
    Expect(TokenKind::LBRACE);

    t.items = ParseEnumerationList();

    Expect(TokenKind::RBRACE);
  });
}

std::vector<nodes::ComponentType*> Parser::ParseComponentTypeList() {
  std::vector<nodes::ComponentType*> list;
  list.push_back(ParseComponentType());

  while (tok_ == TokenKind::COMMA) {
    Consume();
    list.push_back(ParseComponentType());
  }

  return list;
}

nodes::ComponentType* Parser::ParseComponentType() {
  return NewNode<nodes::ComponentType>([&](nodes::ComponentType& c) {
    if (tok_ == TokenKind::ELLIPSIS) {
      Consume();
      c.content = nullptr;  // extension marker
      return;
    }

    /*c.name = */ Expect(TokenKind::IDENT);
    /*c.type = */ ParseAsnType();

    if (tok_ == TokenKind::OPTIONAL) {
      Consume();
      c.optional = true;
    } else if (tok_ == TokenKind::DEFAULT) {
      Consume();
      c.defaultValue = ParseValue();
    }
  });
}

std::vector<nodes::NamedType*> Parser::ParseNamedTypeList() {
  std::vector<nodes::NamedType*> list;
  list.push_back(ParseNamedType());

  while (tok_ == TokenKind::COMMA) {
    Consume();
    list.push_back(ParseNamedType());
  }

  return list;
}

nodes::NamedType* Parser::ParseNamedType() {
  return NewNode<nodes::NamedType>([&](nodes::NamedType& nt) {
    ParseIdent(nt.name);
    nt.type = ParseAsnType();
  });
}

std::vector<nodes::EnumerationItem*> Parser::ParseEnumerationList() {
  std::vector<nodes::EnumerationItem*> list;
  list.push_back(ParseEnumerationItem());

  while (tok_ == TokenKind::COMMA) {
    Consume();
    list.push_back(ParseEnumerationItem());
  }

  return list;
}

nodes::EnumerationItem* Parser::ParseEnumerationItem() {
  return NewNode<nodes::EnumerationItem>([&](nodes::EnumerationItem& e) {
    // IDENTIFIER '(' ... ')' → NamedNumber
    if (tok_ == TokenKind::IDENT && Peek(1).kind == TokenKind::LPAREN) {
      e.value = ParseNamedNumber();
      return;
    }

    if (tok_ == TokenKind::IDENT) {
      e.value = ParseIdent();
      return;
    }

    e.value = ParseValue();
  });
}

nodes::NamedNumber* Parser::ParseNamedNumber() {
  return NewNode<nodes::NamedNumber>([&](nodes::NamedNumber& n) {
    ParseIdent(n.name);
    Expect(TokenKind::LPAREN);

    switch (tok_) {
      case TokenKind::V_INT:
      case TokenKind::V_FLOAT:
        // todo add ParseValueLiteral
        n.value = NewNode<nodes::ValueLiteral>([&](nodes::ValueLiteral& vl) {
          vl.tok = Consume();
        });
        break;
      default:
        n.value = ParseDefinedValue();
        break;
    }

    Expect(TokenKind::RPAREN);
  });
}

nodes::DefinedType* Parser::ParseDefinedType() {
  return NewNode<nodes::DefinedType>([&](nodes::DefinedType& d) {
    ParseIdent(d.name);
    if (tok_ == TokenKind::DOT) {
      Consume();
      // d->module = d->name;
      ParseIdent(d.name);
    }
    if (tok_ == TokenKind::LBRACE) d.parameters = ParseActualParameterList();
  });
}

nodes::ObjectIdentifierValue* Parser::ParseObjectIdentifierValue() {
  return NewNode<nodes::ObjectIdentifierValue>([&](nodes::ObjectIdentifierValue& oid) {
    Expect(TokenKind::LBRACE);

    oid.components.push_back(ParseOidComponent());

    while (tok_ != TokenKind::RBRACE) {
      oid.components.push_back(ParseOidComponent());
    }

    Expect(TokenKind::RBRACE);
  });
}

// ---------- Values ----------

nodes::Value* Parser::ParseValue() {
  return NewNode<nodes::Value>([&](nodes::Value& v) {
    switch (tok_) {
      case TokenKind::LBRACE:
        v.content = ParseObjectIdentifierValue();
        break;
      case TokenKind::V_INT:
      case TokenKind::V_FLOAT:
        // v.content = ParseIntegerValue();
        break;
      default:
        v.content = ParseDefinedValue();
        break;
    }
  });
}

nodes::DefinedValue* Parser::ParseDefinedValue() {
  return NewNode<nodes::DefinedValue>([&](nodes::DefinedValue& d) {
    ParseIdent(d.name);
    if (tok_ == TokenKind::DOT) {
      Consume();
      d.module.emplace(d.name);
      ParseIdent(d.name);
    }
    if (tok_ == TokenKind::LBRACE) d.parameters = ParseActualParameterList();
  });
}

// ---------- OID ----------

nodes::OidComponent* Parser::ParseOidComponent() {
  return NewNode<nodes::OidComponent>([&](nodes::OidComponent& c) {
    switch (tok_) {
      case TokenKind::IDENT:
        c.content = ParseAssignedOidComponent();
        break;
      case TokenKind::V_INT:
      case TokenKind::V_FLOAT:
        Consume();
        // c->content = ParseIntegerValue();
        break;
      default:
        c.content = ParseDefinedValue();
        break;
    }
  });
}

nodes::AssignedOidComponent* Parser::ParseAssignedOidComponent() {
  return NewNode<nodes::AssignedOidComponent>([&](nodes::AssignedOidComponent& a) {
    ParseIdent(a.name);
    Expect(TokenKind::LPAREN);
    switch (tok_) {
      case TokenKind::V_INT:
      case TokenKind::V_FLOAT:
        Consume();
        // a->assignedValue = ParseIntegerValue();
        break;
      default:
        a.assignedValue = ParseDefinedValue();
        break;
    }
    Expect(TokenKind::RPAREN);
  });
}

// ---------- Helpers ----------

std::vector<nodes::Parameter*> Parser::ParseParameterList() {
  std::vector<nodes::Parameter*> v;
  Expect(TokenKind::LBRACE);
  do {
    v.push_back(ParseParameter());
    if (tok_ != TokenKind::COMMA) break;
    Consume();
  } while (true);
  Expect(TokenKind::RBRACE);
  return v;
}

nodes::Parameter* Parser::ParseParameter() {
  return NewNode<nodes::Parameter>([&](nodes::Parameter& p) {
    if (Peek(1).kind == TokenKind::COLON) {
      p.governor = ParseAnyRhs();
      Consume();
    }
    ParseIdent(p.name);
  });
}

std::vector<nodes::ActualParameter*> Parser::ParseActualParameterList() {
  std::vector<nodes::ActualParameter*> v;
  Expect(TokenKind::LBRACE);
  do {
    v.push_back(ParseActualParameter());
    if (tok_ != TokenKind::COMMA) break;
    Consume();
  } while (true);
  Expect(TokenKind::RBRACE);
  return v;
}

nodes::ActualParameter* Parser::ParseActualParameter() {
  return NewNode<nodes::ActualParameter>([&](nodes::ActualParameter& a) {
    a.value = IsTypeStart(tok_) ? static_cast<Node*>(ParseAsnType()) : static_cast<Node*>(ParseValue());
  });
}

nodes::Constraint* Parser::ParseConstraint() {
  return NewNode<nodes::Constraint>([&](nodes::Constraint& c) {
    Expect(TokenKind::LPAREN);
    c.spec = ParseAnyRhs();
    if (tok_ == TokenKind::EXCLAMATION_MARK) c.exception = ParseExceptionSpec();
    Expect(TokenKind::RPAREN);
  });
}

nodes::ExceptionSpec* Parser::ParseExceptionSpec() {
  return NewNode<nodes::ExceptionSpec>([&](nodes::ExceptionSpec& e) {
    Consume();
    e.identification = ParseAnyRhs();
  });
}

Node* Parser::ParseAnyRhs() {
  if (IsTypeStart(tok_)) return ParseAsnType();
  return ParseValue();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Token Parser::Consume() {
  auto tok = queue_[cursor_];
  ++cursor_;

  std::println(stderr, "Consume({}: '{}')", magic_enum::enum_name(tok.kind), tok.On(src_));
  // std::cerr << std::stacktrace::current() << std::endl;
  std::cerr.flush();

  Peek(1);
  tok_ = queue_[cursor_].kind;

  last_consumed_pos_ = tok.range.end;

  return tok;
}

Token Parser::ConsumeInvariant(TokenKind) {
  return Consume();
}

void Parser::Grow(std::uint8_t n) {
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
    std::println(" --> {}", magic_enum::enum_name(token.kind));
    queue_.push_back(token);
    --n;
  }
}

Token& Parser::Peek(std::uint8_t i) {
  const auto idx = cursor_ + i - 1;
  const auto cached = queue_.size();
  if (idx + 1 > cached) {
    Grow(idx - cached + 1);
  }
  return queue_[idx];
}

Token Parser::Expect(TokenKind expected) {
  if (tok_ != expected) [[unlikely]] {
    EmitErrorExpected(magic_enum::enum_name(expected));
    return {};
  }
  return Consume();
}

void Parser::EmitError(const Range& range, std::string&& message) {
  errors_.emplace_back(range, std::move(message));
  std::cerr << std::stacktrace::current() << std::endl;
  throw ParsingCancellationSignal{};
}
void Parser::EmitErrorExpected(std::string_view what) {
  EmitError(Peek(1).range, std::format("expected {}, got {}", what, magic_enum::enum_name(tok_)));
}

//

Token* Parser::TokAlloc(Token&& token) {
  return arena_->Alloc<Token>(std::move(token));
}

// use explicit type in lambda instead of auto for clangd to provide assistance
template <IsNode T, typename Initializer>
  requires std::is_invocable_v<Initializer, T&>
T* Parser::NewNode(Initializer f) {
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
