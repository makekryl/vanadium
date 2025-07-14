#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>

#include "ASTTypes.h"

namespace vanadium::core {
namespace ast {

// NOLINTBEGIN(readability-identifier-naming)
enum class TokenKind : std::uint8_t {
  ILLEGAL,

  kEOF,
  UNTERMINATED,
  MALFORMED,

  // literal_beg,
  COMMENT,
  PREPROC,

  // Identifiers and basic type literals
  // (these tokens stand for classes of literals)
  IDENT,    // main
  INT,      // 12345
  FLOAT,    // 123.45
  STRING,   // "abc"
  BSTRING,  // '101?F'H
  MODIF,    // @fuzzy
  // literal_end,

  // operator_beg,
  // Operators and delimiters
  INC,  // ++
  ADD,  // +
  SUB,  // -
  DEC,  // --
  MUL,  // *
  DIV,  // /

  SHL,     // <<
  ROL,     // <@
  SHR,     // >>
  ROR,     // @>
  CONCAT,  // &

  REDIR,       // ->
  DECODE,      // =>
  ANY,         // ?
  EXCL,        // !
  RANGE,       // ..
  ELIPSIS,     // ...
  ASSIGN,      // :=
  COLONCOLON,  // ::

  EQ,  // ==
  NE,  // !=
  LT,  // <
  LE,  // <=
  GT,  // >
  GE,  // >=

  LPAREN,  // (
  LBRACK,  // [
  LBRACE,  // {
  COMMA,   // ,
  DOT,     // .

  RPAREN,     // )
  RBRACK,     // ]
  RBRACE,     // }
  SEMICOLON,  // ;
  COLON,      // :
  // operator_end,

  // keyword_beg,
  // Keywords
  MOD,  // mod
  REM,  // rem

  AND,  // and
  OR,   // or
  XOR,  // xor
  NOT,  // not

  AND4B,  // and4b
  OR4B,   // or4b
  XOR4B,  // xor4b
  NOT4B,  // not4b

  ADDRESS,
  ALIVE,
  ALL,
  ALT,
  ALTSTEP,
  ANYKW,
  BREAK,
  CASE,
  CHARSTRING,
  CLASS,
  COMPONENT,
  CONST,
  CONTINUE,
  CONTROL,
  CREATE,
  DECMATCH,
  DISPLAY,
  DO,
  ELSE,
  ENCODE,
  ENUMERATED,
  ERROR,
  EXCEPT,
  EXCEPTION,
  EXTENDS,
  EXTENSION,
  EXTERNAL,
  FAIL,
  FALSE,
  FOR,
  FRIEND,
  FROM,
  FUNCTION,
  GOTO,
  GROUP,
  IF,
  IFPRESENT,
  IMPORT,
  IN,
  INCONC,
  INOUT,
  INTERLEAVE,
  LABEL,
  LANGUAGE,
  LENGTH,
  MAP,
  MESSAGE,
  MIXED,
  MODIFIES,
  MODULE,
  MODULEPAR,
  MTC,
  kNaN,
  NOBLOCK,
  NONE,
  kNULL,
  OF,
  OMIT,
  ON,
  OPTIONAL,
  OUT,
  OVERRIDE,
  PARAM,
  PASS,
  PATTERN,
  PORT,
  PRESENT,
  PRIVATE,
  PROCEDURE,
  PUBLIC,
  REALTIME,
  RECORD,
  REGEXP,
  REPEAT,
  RETURN,
  RUNS,
  SELECT,
  SENDER,
  SET,
  SIGNATURE,
  STEPSIZE,
  SYSTEM,
  TEMPLATE,
  TESTCASE,
  TIMER,
  TIMESTAMP,
  TO,
  TRUE,
  TYPE,
  UNION,
  UNIVERSAL,
  UNMAP,
  VALUE,
  VAR,
  VARIANT,
  WHILE,
  WITH,
  keyword_end,
};
// NOLINTEND(readability-identifier-naming)

// NOLINTBEGIN(readability-identifier-naming)
enum class NodeKind : std::uint8_t {
  Root,
  ErrorNode,

  Ident,
  CompositeIdent,
  NameIdent,
  ParametrizedIdent,

  ValueLiteral,
  CompositeLiteral,

  expr_begin_,
  UnaryExpr,
  PostExpr,
  AssignmentExpr,
  BinaryExpr,
  ParenExpr,
  SelectorExpr,
  IndexExpr,
  CallExpr,
  LengthExpr,
  RedirectExpr,
  ValueExpr,
  ParamExpr,
  FromExpr,
  ModifiesExpr,
  RegexpExpr,
  PatternExpr,
  DecmatchExpr,
  DynamicExpr,
  DecodedExpr,
  DefKindExpr,
  ExceptExpr,
  expr_end_,

  stmt_begin_,
  BlockStmt,
  DeclStmt,
  ExprStmt,
  BranchStmt,
  ReturnStmt,
  AltStmt,
  CallStmt,
  ForStmt,
  ForRangeStmt,
  WhileStmt,
  DoWhileStmt,
  IfStmt,
  SelectStmt,
  CaseClause,
  CommClause,
  stmt_end_,

  spec_begin_,
  Field,
  RefSpec,
  StructSpec,
  ListSpec,
  MapSpec,
  EnumSpec,
  BehaviourSpec,
  spec_end,

  decl_begin_,
  ValueDecl,
  Declarator,
  TemplateDecl,
  ModuleParameterGroup,
  FuncDecl,
  ConstructorDecl,
  SignatureDecl,
  SubTypeDecl,
  StructTypeDecl,
  ClassTypeDecl,
  MapTypeDecl,
  EnumTypeDecl,
  BehaviourTypeDecl,
  PortTypeDecl,
  PortAttribute,
  PortMapAttribute,
  ComponentTypeDecl,
  decl_end_,

  module_begin_,
  Module,
  ModuleDef,
  ControlPart,
  ImportDecl,
  GroupDecl,
  FriendDecl,
  module_end,

  misc_begin_,
  LanguageSpec,
  RestrictionSpec,
  RunsOnSpec,
  SystemSpec,
  MtcSpec,
  ReturnSpec,
  FormalPars,
  FormalPar,
  WithSpec,
  WithStmt,
  RedirectToIndex,
  misc_end_,
};
// NOLINTEND(readability-identifier-naming)

struct Token {
  TokenKind kind;
  ast::Range range;

  [[nodiscard]] std::string_view On(std::string_view src) const noexcept {
    return range.String(src);
  }
};

struct Node {
  Node(NodeKind nkind_) : nkind(nkind_) {}

  Node* parent;
  ast::Range nrange;
  const NodeKind nkind;

  void Accept(const NodeInspector& inspector) const;

  [[nodiscard]] bool Contains(const Node* other) const {
    return nrange.Contains(other->nrange);
  }

  [[nodiscard]] std::string_view On(std::string_view src) const noexcept {
    return nrange.String(src);
  }

  template <typename DerivedNode>
    requires std::is_base_of_v<Node, DerivedNode>
  DerivedNode* As() {
    return static_cast<DerivedNode*>(this);
  }

  template <typename DerivedNode>
    requires std::is_base_of_v<Node, DerivedNode>
  [[nodiscard]] const DerivedNode* As() const {
    return static_cast<const DerivedNode*>(this);
  }
};

namespace nodes {

struct Expr : Node {
  Expr(NodeKind nkind) : Node(nkind) {}
};
struct Stmt : Node {
  Stmt(NodeKind nkind) : Node(nkind) {}
};
struct Decl : Node {
  Decl(NodeKind nkind) : Node(nkind) {}
};
struct TypeSpec : Node {
  TypeSpec(NodeKind nkind) : Node(nkind) {}
};

struct DeclStmt : Stmt {
  DeclStmt() : Stmt(NodeKind::DeclStmt) {}

  Decl* decl;

  void Accept(const NodeInspector& inspector) const {
    Inspect(decl, inspector);
  }
};
struct ExprStmt : Stmt {
  ExprStmt() : Stmt(NodeKind::ExprStmt) {}

  Expr* expr;

  void Accept(const NodeInspector& inspector) const {
    Inspect(expr, inspector);
  }
};

struct Ident : Expr {
  Ident() : Expr(NodeKind::Ident) {}

  [[nodiscard]] constexpr bool IsName() const noexcept {
    return nkind == NodeKind::NameIdent;
  }

  [[nodiscard]] constexpr bool IsComposite() const noexcept {
    return nkind == NodeKind::CompositeIdent;
  }

  void Accept(const NodeInspector&) const {};

 protected:
  Ident(NodeKind nkind) : Expr(nkind) {}
};
struct CompositeIdent : Ident {
  CompositeIdent() : Ident(NodeKind::CompositeIdent) {}

  Token tok1;
  Token tok2;
};
struct NameIdent : Ident {
  NameIdent() : Ident(NodeKind::NameIdent) {}
};

#include "gen/ASTNodes.inc"

}  // namespace nodes

inline void Inspect(const Node* node, const NodeInspector& inspector) {
  if (inspector(node)) {
    node->Accept(inspector);
  }
}

}  // namespace ast
}  // namespace vanadium::core
