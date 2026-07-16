#pragma once

#include <string_view>
#include <vector>

#include "vanadium/ast/ASTNodes.h"
#include "vanadium/ast/ASTTypes.h"
#include "vanadium/ast/OriginMap.h"

namespace vanadium {

namespace core {
struct SourceFile;
}

namespace ast {

struct RootNode : Node {
  RootNode() : Node(NodeKind::RootNode) {}

  core::SourceFile* file{nullptr};
  std::vector<Node*> nodes;

  void Accept(const NodeInspector& inspector) const {
    for (auto* node : nodes) {
      Inspect(node, inspector);
    }
  }
};

enum class SourceLanguage : std::uint8_t {
  kTTCN,
  kASN,
  // TODO: kProto
};

struct AST {
  std::string_view src;
  RootNode* root;
  LineMapping lines;
  std::vector<SyntaxError> errors;
  SourceLanguage language;
  OriginMap* origins{nullptr};

  [[nodiscard]] std::string_view Text(const Node* n) const noexcept {
    return n->On(src);
  }

  [[nodiscard]] std::string_view Text(const Node& n) const noexcept {
    return n.On(src);
  }

  [[nodiscard]] std::string_view Text(const Token& t) const noexcept {
    return t.On(src);
  }

  [[nodiscard]] std::string_view Text(const Range& r) const noexcept {
    return r.String(src);
  }

  [[nodiscard]] std::optional<ExpansionPointSpan> GetProvenance(const Node* n) const {
    if (!origins) {
      return std::nullopt;
    }
    return origins->Lookup(n);
  }
};

}  // namespace ast
};  // namespace vanadium
