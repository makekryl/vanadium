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
#include <magic_enum/magic_enum.hpp>
#include <print>
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

nodes::SymbolReference* Parser::ParseSymbolReference() {
  return NewNode<nodes::SymbolReference>([&](nodes::SymbolReference& m) {
    ParseIdent(m.ident);
    if (tok_ == TokenKind::LBRACE) {
      ConsumeInvariant(TokenKind::LBRACE);
      Expect(TokenKind::RBRACE);
      m.parametrized = true;
    }
  });
}

nodes::Module* Parser::ParseModule() {
  return NewNode<nodes::Module>([&](nodes::Module& m) {
    ParseIdent(m.name);

    if (tok_ == TokenKind::LBRACE) {
      ConsumeInvariant(TokenKind::LBRACE);
      m.components = ParseDefinitiveComponents();
      Expect(TokenKind::RBRACE);
    }

    Expect(TokenKind::DEFINITIONS);

    switch (tok_) {
      case TokenKind::EXPLICIT:
      case TokenKind::IMPLICIT:
      case TokenKind::AUTOMATIC:
        m.tags = Consume();
        Expect(TokenKind::TAGS);
        break;
      default:
        m.tags = {.kind = TokenKind::kSentinel};
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
      m.defs.push_back(ParseDefinition());
    }

    Expect(TokenKind::END);
  });
}

nodes::DefinitiveComponents* Parser::ParseDefinitiveComponents() {
  return NewNode<nodes::DefinitiveComponents>([&](nodes::DefinitiveComponents& m) {
    while (tok_ != TokenKind::RBRACE) {
      m.list.emplace_back(NewNode<nodes::DefinitiveComponent>([&](nodes::DefinitiveComponent& x) {
        if (tok_ == TokenKind::IDENT) {
          x.name = ParseIdent();
          if (tok_ == TokenKind::LPAREN) {
            ConsumeInvariant(TokenKind::LPAREN);
            x.number = Expect(TokenKind::V_INT);
            Expect(TokenKind::RPAREN);
          }
        } else {
          x.number = Expect(TokenKind::V_INT);
        }
      }));
    }
  });
}

nodes::ModuleExports* Parser::ParseModuleExports() {
  return NewNode<nodes::ModuleExports>([&](nodes::ModuleExports& m) {
    ConsumeInvariant(TokenKind::EXPORTS);
    if (tok_ == TokenKind::ALL) {
      ConsumeInvariant(TokenKind::ALL);
      m.all = true;
    } else {
      m.list = ParseSymbolReferenceList();
    }
    Expect(TokenKind::SEMICOLON);
  });
}

nodes::ModuleImports* Parser::ParseModuleImports() {
  return NewNode<nodes::ModuleImports>([&](nodes::ModuleImports& m) {
    ConsumeInvariant(TokenKind::IMPORTS);
    while (tok_ == TokenKind::IDENT) {
      m.list.push_back(ParseSymbolsFromModule());
    }
    Expect(TokenKind::SEMICOLON);
  });
}

nodes::SymbolsFromModule* Parser::ParseSymbolsFromModule() {
  return NewNode<nodes::SymbolsFromModule>([&](nodes::SymbolsFromModule& m) {
    m.list = ParseSymbolReferenceList();
    Expect(TokenKind::FROM);
    ParseIdent(m.provider);
  });
}

std::vector<nodes::SymbolReference*> Parser::ParseSymbolReferenceList() {
  std::vector<nodes::SymbolReference*> v;
  v.push_back(ParseSymbolReference());
  while (tok_ == TokenKind::COMMA) {
    Consume();
    v.push_back(ParseSymbolReference());
  }
  return v;
}

Node* Parser::ParseDefinition() {
  Consume();
  return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Token Parser::Consume() {
  auto tok = queue_[cursor_];
  ++cursor_;

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
