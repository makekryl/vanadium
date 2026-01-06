#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>

#include "vanadium/asn1/ast/Asn1ASTTypes.h"

namespace vanadium::asn1::ast {

// NOLINTBEGIN(readability-identifier-naming)
enum class TokenKind : std::uint8_t {
  kSentinel,

  ILLEGAL,

  kEOF,
  UNTERMINATED,
  MALFORMED,

  //

  COMMENT,

  //

  IDENT,          // main
  V_INT,          // 12345
  V_FLOAT,        // 123.45
  V_STRING,       // "abc"
  V_BITSTRING,    // '101?F'B
  V_HEXSTRING,    // '101?F'H
  V_OCTETSTRING,  // '101?F'O
  V_TRUE,         // true
  V_FALSE,        // false

  //

  DOT,               // .
  COMMA,             // ,
  COLON,             // :
  SEMICOLON,         // ;
  VERTICALBAR,       // |
  EXCLAMATION_MARK,  // !

  REF,  // &

  RANGE,     // ..
  ELLIPSIS,  // ...
  ASSIGN,    // ::=

  LPAREN,   // (
  RPAREN,   // )
  LBRACE,   // {
  RBRACE,   // }
  LBRACK,   // [
  RBRACK,   // ]
  DLBRACK,  // [[
  DRBRACK,  // ]]

  //

  DEFINITIONS,
  EXPLICIT,
  IMPLICIT,
  AUTOMATIC,
  TAGS,
  EXTENSIBILITY,
  IMPLIED,
  EXPORTS,
  IMPORTS,
  FROM,

  ALL,

  BEGIN,
  END,

  SEQUENCE,
  SET,
  CHOICE,
  ENUMERATED,
  CLASS,

  INTEGER,
  REAL,
  STRING,
  BIT,
  OCTET,
  BOOLEAN,

  OBJECT,
  IDENTIFIER,

  UNIVERSAL,
  APPLICATION,
  PRIVATE,

  DEFAULT,
  OPTIONAL,
  UNIQUE,
  SIZE,
  CONTAINING,
  WITH,

  OF,

  kNULL,
};
// NOLINTEND(readability-identifier-naming)

// NOLINTBEGIN(readability-identifier-naming)
enum class NodeKind : std::uint8_t {
  RootNode,

  Ident,

  Module,
  ModuleExports,
  ModuleImports,
  SymbolsFromModule,
  Symbol,
  Assignment,
  TypeAssignment,
  ValueAssignment,
  ParameterizedAssignment,
  ObjectClassAssignment,
  Tag,
  BuiltinTypeIdent,
  AsnType,
  DefinedType,
  SequenceType,
  SetType,
  ComponentType,
  SequenceOfType,
  SetOfType,
  ChoiceType,
  EnumeratedType,
  EnumerationItem,
  NamedNumber,
  NamedType,
  Constraint,
  ExceptionSpec,
  ElementSetSpec,
  ExtensionSpec,
  ElementSet,
  Intersection,
  Elements,
  Value,
  BuiltinValue,
  ObjectIdentifierValue,
  OidComponent,
  AssignedOidComponent,
  ValueLiteral,
  DefinedValue,
  Parameter,
  ObjectClass,
  DefinedObjectClass,
  ObjectClassDefn,
  Object,
  FieldSpec,
  ObjectSetSpec,
  DefinedObject,
  ParameterizedObject,
  ActualParameter,
  ObjectSet,
  WithSyntaxSpec,
  SyntaxToken,
  OptionalGroup,
};
// NOLINTEND(readability-identifier-naming)

struct Token {
  TokenKind kind;
  Range range;

  [[nodiscard]] std::string_view On(std::string_view src) const noexcept {
    return range.String(src);
  }
};

struct Node;

template <typename T>
concept IsNode = std::is_base_of_v<Node, T>;

struct Node {
  Node(NodeKind nkind_) : nkind(nkind_) {}

  Node* parent;
  Range nrange;
  const NodeKind nkind;

  void Accept(const NodeInspector& inspector) const;

  [[nodiscard]] bool Contains(const Node* other) const {
    return nrange.Contains(other->nrange);
  }

  [[nodiscard]] std::string_view On(std::string_view src) const noexcept {
    return nrange.String(src);
  }

  template <IsNode DerivedNode>
  DerivedNode* As() {
    return static_cast<DerivedNode*>(this);
  }

  template <IsNode DerivedNode>
  [[nodiscard]] const DerivedNode* As() const {
    return static_cast<const DerivedNode*>(this);
  }
};

namespace nodes {

#include "vanadium/asn1/ast/gen/ASTNodes.inc"

}  // namespace nodes

inline void Inspect(const Node* node, const NodeInspector& inspector) {
  if (inspector(node)) {
    node->Accept(inspector);
  }
}

}  // namespace vanadium::asn1::ast
