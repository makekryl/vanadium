#pragma once

#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

#include "vanadium/ast/ASTNodes.h"
#include "vanadium/ast/ASTTypes.h"

namespace vanadium {
namespace core {
struct SourceFile;
}

namespace ast {

struct ExpansionPoint {
  Range range;
  const void* source_node;
  const void* filekey;  // SourceFile*

  [[nodiscard]] const core::SourceFile* SourceFile() const {
    return static_cast<const core::SourceFile*>(filekey);
  }
};

using ExpansionPointSpan = std::span<const ExpansionPoint>;

struct OriginMap {
  std::unordered_map<const Node*, std::vector<ExpansionPoint>> provenance;

  void Put(const Node* n, std::vector<ExpansionPoint> stack) {
    provenance.emplace(n, std::move(stack));
  }
  void Put(const Node* n, ExpansionPoint ep) {
    provenance.emplace(n, std::vector<ExpansionPoint>{std::move(ep)});
  }

  [[nodiscard]] std::optional<ExpansionPointSpan> Lookup(const Node* n) const {
    if (auto it = provenance.find(n); it != provenance.end()) {
      return it->second;
    }
    return std::nullopt;
  }
};

}  // namespace ast
}  // namespace vanadium
