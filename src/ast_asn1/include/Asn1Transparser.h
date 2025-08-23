#pragma once

#include <string_view>
#include <vector>

#include "AST.h"
#include "ASTNodes.h"
#include "ASTTypes.h"
#include "Arena.h"
#include "Asn1AST.h"
#include "Asn1Scanner.h"

namespace vanadium::asn1::ast {
namespace parser {

class Transparser {
 public:
  Transparser(lib::Arena& arena, std::string_view src);

  std::pair<ttcn_ast::RootNode*, std::string_view> ParseRoot();
  std::vector<ttcn_ast::SyntaxError>&& GetErrors() noexcept;
  std::vector<ttcn_ast::pos_t>&& ExtractLineMapping() noexcept;

 private:
  ttcn_ast::RootNode* ParseRootInternal();

  void RewriteIdentName(const Token&);
  ttcn_ast::nodes::Ident* ParseName();
  void ParseName(std::optional<ttcn_ast::nodes::Ident>&);

  ttcn_ast::nodes::Module* ParseModule();
  ttcn_ast::nodes::Definition* ParseDefinition();

  ttcn_ast::nodes::ValueDecl* ParseConstDecl(ttcn_ast::nodes::Ident&& name);

  ttcn_ast::nodes::TypeSpec* ParseTypeSpec();

  void ParseFields(std::vector<ttcn_ast::nodes::Field*>&, TokenKind term = TokenKind::RBRACE);
  ttcn_ast::nodes::Field* ParseField();

  void ParseEnumValues(std::vector<ttcn_ast::nodes::Expr*>&);

  ttcn_ast::nodes::LengthExpr* ParseSizeConstraint();
  ttcn_ast::nodes::ParenExpr* ParseValueConstraint();

  ttcn_ast::nodes::Ident VerIdent(std::uint32_t);

  //

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

  template <ttcn_ast::IsNode T, typename Initializer>
    requires std::is_invocable_v<Initializer, T&>
  T* NewNode(Initializer f);

  ttcn_ast::Node* last_node_{nullptr};

  std::vector<ttcn_ast::SyntaxError> errors_;

  TokenKind tok_;
  std::vector<Token> queue_;
  ttcn_ast::pos_t cursor_{};
  ttcn_ast::pos_t last_consumed_pos_;

  Scanner scanner_;
  std::string_view src_;
  std::uint32_t max_ver_{1};

  lib::Arena* arena_;
};

}  // namespace parser

inline ttcn_ast::AST Parse(lib::Arena& arena, std::string_view src) {
  parser::Transparser parser(arena, src);
  auto [root, new_src] = parser.ParseRoot();
  return {
      .src = new_src,
      .root = root,
      .lines = parser.ExtractLineMapping(),
      .errors = parser.GetErrors(),
  };
}

}  // namespace vanadium::asn1::ast
