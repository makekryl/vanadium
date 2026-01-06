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
  nodes::Module* ParseModule();
  nodes::ModuleExports* ParseModuleExports();
  nodes::ModuleImports* ParseModuleImports();
  nodes::SymbolsFromModule* ParseSymbolsFromModule();
  nodes::Symbol* ParseSymbol();
  nodes::Assignment* ParseAssignment();
  nodes::TypeAssignment* ParseTypeAssignment();
  nodes::ValueAssignment* ParseValueAssignment();
  nodes::ParameterizedAssignment* ParseParameterizedAssignment();
  nodes::Tag* ParseTag();
  nodes::AsnType* ParseAsnType();
  Node* ParseTypeBase();
  nodes::SequenceType* ParseSequenceType();
  nodes::SetType* ParseSetType();
  nodes::ChoiceType* ParseChoiceType();
  nodes::EnumeratedType* ParseEnumeratedType();
  std::vector<nodes::ComponentType*> ParseComponentTypeList();
  nodes::ComponentType* ParseComponentType();
  std::vector<nodes::NamedType*> ParseNamedTypeList();
  nodes::NamedType* ParseNamedType();
  std::vector<nodes::EnumerationItem*> ParseEnumerationList();
  nodes::EnumerationItem* ParseEnumerationItem();
  nodes::NamedNumber* ParseNamedNumber();
  nodes::DefinedType* ParseDefinedType();
  nodes::ObjectIdentifierValue* ParseObjectIdentifierValue();
  nodes::Value* ParseValue();
  nodes::DefinedValue* ParseDefinedValue();
  nodes::OidComponent* ParseOidComponent();
  nodes::AssignedOidComponent* ParseAssignedOidComponent();
  std::vector<nodes::Parameter*> ParseParameterList();
  nodes::Parameter* ParseParameter();
  std::vector<nodes::ActualParameter*> ParseActualParameterList();
  nodes::ActualParameter* ParseActualParameter();
  nodes::Constraint* ParseConstraint();
  nodes::ExceptionSpec* ParseExceptionSpec();
  Node* ParseAnyRhs();
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
