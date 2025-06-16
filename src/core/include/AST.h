#pragma once

#include <string_view>
#include <vector>

#include "ASTNodes.h"
#include "ASTTypes.h"

namespace vanadium::core {

struct SourceFile;

namespace ast {

struct RootNode : Node {
  RootNode() : Node(NodeKind::Root) {}

  SourceFile* file{nullptr};
  std::vector<Node*> nodes;

  void Accept(const NodeInspector& inspector) const {
    for (auto* node : nodes) {
      Inspect(node, inspector);
    }
  }
};

struct AST {
  std::string_view src;
  RootNode* root;
  LineMapping lines;
  std::vector<SyntaxError> errors;

  [[nodiscard]] std::string_view Text(const Node* n) const noexcept {
    return n->On(src);
  }
};

}  // namespace ast
};  // namespace vanadium::core
