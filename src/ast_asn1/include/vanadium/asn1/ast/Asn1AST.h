#pragma once

#include <string_view>
#include <vector>

#include "vanadium/asn1/ast/Asn1ASTNodes.h"
#include "vanadium/asn1/ast/Asn1ASTTypes.h"

namespace vanadium::asn1 {
namespace ast {

struct RootNode : Node {
  RootNode() : Node(NodeKind::RootNode) {}

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

  [[nodiscard]] std::string_view Text(const Node& n) const noexcept {
    return n.On(src);
  }

  [[nodiscard]] std::string_view Text(const Token* t) const noexcept {
    return t->On(src);
  }

  [[nodiscard]] std::string_view Text(const Range& r) const noexcept {
    return r.String(src);
  }
};

}  // namespace ast
};  // namespace vanadium::asn1
