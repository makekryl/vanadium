#pragma once

#include <string_view>
#include <vector>

#include "AST.h"
#include "ASTTypes.h"
#include "Arena.h"
#include "Asn1AST.h"
#include "Asn1Scanner.h"

namespace vanadium::asn1::ast {
namespace parser {

class Transparser {
 public:
  Transparser(lib::Arena& arena, std::string_view src);

  ttcn_ast::RootNode* ParseRoot();
  std::vector<ttcn_ast::SyntaxError>&& GetErrors() noexcept;
  std::vector<ttcn_ast::pos_t>&& ExtractLineMapping() noexcept;

 private:
  ttcn_ast::Node* Parse();

  Token Consume();
  Token ConsumeInvariant(TokenKind);
  void Grow(std::uint8_t n);
  Token& Peek(std::uint8_t i);
  std::string_view Lit(std::uint8_t i);
  ttcn_ast::pos_t Pos(std::uint8_t i);
  Token Expect(TokenKind expected);

  void EmitError(const ttcn_ast::Range& range, std::string&& message);
  void EmitErrorExpected(std::string_view what);

  Token* TokAlloc(Token&& token);

  template <typename T, typename Initializer>
    requires std::is_invocable_v<Initializer, T&>
  T* NewNode(Initializer f);

  ttcn_ast::Node* last_node_{nullptr};

  bool seen_closing_brace_;

  std::vector<ttcn_ast::SyntaxError> errors_;

  TokenKind tok_;
  std::vector<Token> queue_;
  ttcn_ast::pos_t last_consumed_pos_;

  Scanner scanner_;
  std::string_view src_;

  lib::Arena* arena_;
};

}  // namespace parser

inline core::ast::AST Parse(lib::Arena& arena, std::string_view src) {
  parser::Transparser parser(arena, src);
  auto* root = parser.ParseRoot();
  return {
      .src = src,
      .root = root,
      .lines = parser.ExtractLineMapping(),
      .errors = parser.GetErrors(),
  };
}

}  // namespace vanadium::asn1::ast
