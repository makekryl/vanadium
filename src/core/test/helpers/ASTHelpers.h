#pragma once

#include "AST.h"

namespace vanadium {
namespace ut {

inline bool IsInHierarchyOf(const core::ast::Node* node, const core::ast::RootNode& root) {
  for (const auto* par = node->parent; par != nullptr; par = par->parent) {
    if (std::ranges::find(root.nodes, par) != root.nodes.end()) {
      return true;
    }
  }
  return false;
}

inline bool IsInHierarchyOf(const core::ast::Node* node, const core::ast::Node* root) {
  for (const auto* par = node->parent; par != nullptr; par = par->parent) {
    if (par == root) {
      return true;
    }
  }
  return false;
}

}  // namespace ut
}  // namespace vanadium
