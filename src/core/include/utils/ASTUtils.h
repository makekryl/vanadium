#pragma once

#include "AST.h"
#include "ASTNodes.h"
#include "ASTTypes.h"

namespace vanadium::core::ast {
namespace utils {

const Range& GetActualNameRange(const Node* n);

template <IsNode ConcreteNode>
inline const ConcreteNode* GetPredecessor(const Node* n) {
  for (const Node* c = n->parent; c != nullptr; c = c->parent) {
    if (c->nkind == ConcreteNode::kKind) {
      return c->As<ConcreteNode>();
    }
  }
  return nullptr;
}

inline const nodes::Ident* GetEnumValueNamePart(const nodes::Expr* n) {
  switch (n->nkind) {
    case NodeKind::Ident:
      return n->As<nodes::Ident>();
    case NodeKind::CallExpr:
      return n->As<nodes::CallExpr>()->fun->As<nodes::Ident>();
    default:
      return nullptr;
  }
}

inline const nodes::FormalPars* GetCallableDeclParams(const nodes::Decl* decl) {
  switch (decl->nkind) {
    case NodeKind::FuncDecl:
      return decl->As<nodes::FuncDecl>()->params;
    case NodeKind::ConstructorDecl:
      return decl->As<nodes::ConstructorDecl>()->params;
    case NodeKind::TemplateDecl:
      return decl->As<nodes::TemplateDecl>()->params;
    default:
      return nullptr;
  }
}

inline const nodes::RestrictionSpec* GetTemplateRestriction(const Node* n) {
  switch (n->nkind) {
    case NodeKind::ValueDecl:
      return n->As<nodes::ValueDecl>()->template_restriction;
    case NodeKind::TemplateDecl:
      return n->As<nodes::TemplateDecl>()->restriction;
    case NodeKind::FormalPar:
      return n->As<nodes::FormalPar>()->restriction;
    case NodeKind::ReturnSpec:
      return n->As<nodes::ReturnSpec>()->restriction;
    default:
      return nullptr;
  }
}

inline const std::vector<nodes::ParenExpr*>* GetArrayDef(const Node* n) {
  switch (n->nkind) {
    case NodeKind::Field:
      return &n->As<nodes::Field>()->arraydef;
    case NodeKind::Declarator:
      return &n->As<nodes::Declarator>()->arraydef;
    case NodeKind::FormalPar:
      return &n->As<nodes::FormalPar>()->arraydef;
    default:
      return nullptr;
  }
}

inline const std::vector<nodes::Field*>* GetStructFields(const Node* n) {
  switch (n->nkind) {
    case ast::NodeKind::StructTypeDecl:
      return &n->As<ast::nodes::StructTypeDecl>()->fields;
    case ast::NodeKind::StructSpec:
      return &n->As<ast::nodes::StructSpec>()->fields;
    default:
      return nullptr;
  }
}

inline const Node* TraverseSelectorExpressionStart(const nodes::SelectorExpr* se) {
  while (se->x->nkind == NodeKind::SelectorExpr) {
    se = se->x->As<nodes::SelectorExpr>();
  }
  return se->x;
}

inline SourceFile* SourceFileOf(const Node* n) {
  while (n->nkind != NodeKind::RootNode) {
    n = n->parent;
  }
  return n->As<RootNode>()->file;
}

const Node* GetNodeAt(const AST& ast, pos_t pos);

}  // namespace utils
}  // namespace vanadium::core::ast
