#pragma once

#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>

#include <string_view>
#include <vector>

#include "Arena.h"
#include "vanadium/asn1/ast/Asn1AST.h"
#include "vanadium/asn1/ast/Asn1ASTNodes.h"
#include "vanadium/asn1/ast/Asn1Scanner.h"

namespace vanadium::asn1::ast {
namespace parser {

class Parser {
 public:
  Parser(lib::Arena& arena, std::string_view src);

  RootNode* ParseRoot();
  std::vector<SyntaxError>&& GetErrors() noexcept;
  std::vector<pos_t>&& ExtractLineMapping() noexcept;

 private:
  RootNode* ParseRootImpl();

  Node* Parse();
  //
  void ParseIdent(nodes::Ident&);
  nodes::Ident* ParseIdent();
  //
  nodes::SymbolReference* ParseSymbolReference();
  //
  nodes::Module* ParseModule();
  nodes::DefinitiveComponents* ParseDefinitiveComponents();
  nodes::ModuleExports* ParseModuleExports();
  nodes::ModuleImports* ParseModuleImports();
  nodes::SymbolsFromModule* ParseSymbolsFromModule();
  std::vector<nodes::SymbolReference*> ParseSymbolReferenceList();
  //
  Node* ParseDefinition();
  //

  Token Consume();
  Token ConsumeInvariant(TokenKind);
  void Grow(std::uint8_t n);
  Token& Peek(std::uint8_t i);
  std::string_view Lit(std::uint8_t i);
  pos_t Pos(std::uint8_t i);
  Token Expect(TokenKind expected);

  void EmitError(const Range& range, std::string&& message);
  void EmitErrorExpected(std::string_view what);

  Token* TokAlloc(Token&& token);

  template <IsNode T, typename Initializer>
    requires std::is_invocable_v<Initializer, T&>
  T* NewNode(Initializer f);

  Node* last_node_{nullptr};

  std::vector<SyntaxError> errors_;

  TokenKind tok_;
  std::vector<Token> queue_;
  pos_t cursor_{};
  pos_t last_consumed_pos_;

  Scanner scanner_;
  std::string_view src_;
  std::uint32_t max_ver_{1};

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

}  // namespace vanadium::asn1::ast
