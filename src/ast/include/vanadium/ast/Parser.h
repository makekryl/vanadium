#pragma once

#include <string_view>
#include <type_traits>

#include <vanadium/lib/Arena.h>
#include <vanadium/lib/StaticSet.h>

#include "vanadium/ast/AST.h"
#include "vanadium/ast/ASTNodes.h"
#include "vanadium/ast/ASTTypes.h"
#include "vanadium/ast/Scanner.h"

namespace vanadium::ast {
namespace parser {

class Parser {
 public:
  Parser(lib::Arena& arena, std::string_view src);

  RootNode* ParseRoot();
  std::vector<SyntaxError>&& GetErrors() noexcept;
  std::vector<pos_t>&& ExtractLineMapping() noexcept;

 private:
  Node* Parse();
  //
  auto TryParseName(std::invocable auto parse, std::invocable auto failure);
  nodes::Ident* ParseName();
  void ParseName(std::optional<nodes::Ident>&);
  //
  nodes::Ident* ParseAnyIdent();
  nodes::Ident* ParseExpectedIdent(TokenKind kind);
  //
  nodes::Module* ParseModule();
  nodes::Definition* ParseDefinition();
  nodes::Decl* ParseModulePar();
  nodes::ControlPart* ParseControlPart();
  //
  nodes::ImportDecl* ParseImport();
  nodes::DefKindExpr* ParseImportStmt();
  std::vector<nodes::Expr*> ParseExceptStmts();
  nodes::DefKindExpr* ParseExceptStmt();
  //
  nodes::GroupDecl* ParseGroup();
  nodes::FriendDecl* ParseFriend();
  //
  nodes::LanguageSpec* ParseLanguageSpec();
  //
  nodes::Decl* ParseTypeDecl();
  nodes::SubTypeDecl* ParseSubTypeDecl();
  nodes::PortTypeDecl* ParsePortTypeDecl();
  Node* ParsePortAttribute();
  nodes::ComponentTypeDecl* ParseComponentTypeDecl();
  nodes::StructTypeDecl* ParseStructTypeDecl();
  nodes::ClassTypeDecl* ParseClassTypeDecl();
  nodes::MapTypeDecl* ParseMapTypeDecl();
  nodes::EnumTypeDecl* ParseEnumTypeDecl();
  nodes::Expr* ParseEnumValue();
  nodes::BehaviourTypeDecl* ParseBehaviourTypeDecl();
  //
  nodes::Field* ParseField();
  nodes::TypeSpec* ParseTypeSpec();
  nodes::StructSpec* ParseStructSpec();
  nodes::MapSpec* ParseMapSpec();
  nodes::EnumSpec* ParseEnumSpec();
  nodes::ListSpec* ParseListSpec();
  nodes::BehaviourSpec* ParseBehaviourSpec();
  nodes::TemplateDecl* ParseTemplateDecl();
  nodes::ValueDecl* ParseValueDecl();
  nodes::RestrictionSpec* ParseRestrictionSpec();
  std::vector<nodes::Declarator*> ParseDeclList();
  nodes::Declarator* ParseDeclarator();
  nodes::FuncDecl* ParseFuncDecl();
  nodes::FuncDecl* ParseExtFuncDecl();
  nodes::ConstructorDecl* ParseConstructorDecl();
  nodes::SignatureDecl* ParseSignatureDecl();
  //
  nodes::ReturnSpec* ParseReturn();
  //
  nodes::FormalPars* ParseFormalPars();
  nodes::FormalPar* ParseFormalPar();
  //
  nodes::FormalPars* ParseTypeFormalPars();
  nodes::FormalPar* ParseTypeFormalPar();
  //
  std::vector<nodes::ParenExpr*> ParseArrayDefs();
  nodes::ParenExpr* ParseArrayDef();
  //
  nodes::RunsOnSpec* ParseRunsOn();
  nodes::SystemSpec* ParseSystem();
  nodes::MtcSpec* ParseMtc();
  //
  nodes::WithSpec* ParseWith();
  nodes::WithStmt* ParseWithStmt();
  nodes::Expr* ParseWithQualifier();
  //
  std::vector<nodes::Expr*> ParseRefList();
  nodes::Expr* ParseTypeRef();
  //
  nodes::Stmt* ParseStmt();
  nodes::BlockStmt* ParseBlockStmt();
  nodes::Stmt* ParseForLoop();
  //
  nodes::WhileStmt* ParseWhileLoop();
  nodes::DoWhileStmt* ParseDoWhileLoop();
  nodes::IfStmt* ParseIfStmt();
  nodes::SelectStmt* ParseSelect();
  nodes::CaseClause* ParseCaseClause();
  nodes::CommClause* ParseAltGuard();
  //
  nodes::Stmt* ParseSimpleStmt();
  //
  nodes::Expr* ParseExpr();
  nodes::Expr* ParseBinaryExpr(int prec);
  nodes::Expr* ParsePostfixExpr();
  nodes::Expr* ParseUnaryExpr();
  nodes::Expr* ParsePrimaryExpr();
  //
  nodes::Expr* ParseOperand();
  //
  nodes::SelectorExpr* ParseSelectorExpr(nodes::Expr* x);
  nodes::IndexExpr* ParseIndexExpr(nodes::Expr* x);
  nodes::CallExpr* ParseCallExpr(nodes::Expr* x);
  nodes::LengthExpr* ParseLengthExpr(nodes::Expr* x);
  nodes::RedirectExpr* ParseRedirect(nodes::Expr* x);
  //
  std::vector<nodes::Expr*> ParseExprList();
  //
  nodes::Ident* ParseRef();
  nodes::ParenExpr* ParseParenExpr();
  //
  nodes::CompositeIdent* ParseUniversalCharstring();
  nodes::CompositeLiteral* ParseCompositeLiteral();
  //
  nodes::RegexpExpr* ParseCallRegexp();
  nodes::PatternExpr* ParseCallPattern();
  //
  nodes::DecmatchExpr* ParseCallDecmatch();
  //
  nodes::Expr* ParseModifier();
  nodes::DynamicExpr* ParseDynamicModifier();
  nodes::DecodedExpr* ParseDecodedModifier();
  //
  nodes::ParenExpr* TryParseTypeParameters();
  nodes::Expr* TryParseTypeParameter();
  nodes::Ident* TryParseTypeIdent();

  Token Consume();
  Token ConsumeInvariant(TokenKind);
  void Grow(std::uint8_t n);
  Token& Peek(std::uint8_t i);
  std::string_view Lit(std::uint8_t i);
  Token Expect(TokenKind expected);
  void MaybeExpectSemi();
  void ExpectSemi();
  void ExpectSemiAfter(Node*);

  template <std::size_t N>
  void Advance(const lib::StaticSet<TokenKind, N>& to);

  void Mark();
  void Commit();
  void Reset();
  [[nodiscard]] bool IsSpeculating() const noexcept;

  void EmitError(const ast::Range& range, std::string&& message);
  void EmitErrorExpected(std::string_view what);

  Token* TokAlloc(Token&& token);

  template <IsNode T, typename Initializer>
    requires std::is_invocable_v<Initializer, T&>
  T* NewNode(Initializer f);

  template <IsNode ConcreteNode>
  ConcreteNode* NewErrorNode();
  template <IsNode ConcreteNode>
  ConcreteNode* NewErrorNode(const Token&);

  struct {
    bool is_inside_external_scope{false};
  } ext_state_;

  Node* last_node_{nullptr};

  bool seen_closing_brace_;

  std::vector<SyntaxError> errors_;

  ast::pos_t cursor_{};
  std::vector<ast::pos_t> markers_;

  TokenKind tok_;
  std::vector<Token> queue_;
  ast::pos_t last_consumed_pos_;

  Scanner scanner_;
  std::string_view src_;

  lib::Arena* arena_;
};

}  // namespace parser

inline AST Parse(lib::Arena& arena, std::string_view src) {
  parser::Parser parser(arena, src);
  auto* root = parser.ParseRoot();
  return {
      .src = src,
      .root = root,
      .lines = parser.ExtractLineMapping(),
      .errors = parser.GetErrors(),
  };
}

}  // namespace vanadium::ast
