#pragma once

#include "vanadium/core/Program.h"
#include "vanadium/core/Semantic.h"

namespace vanadium::core::semantic {
namespace utils {

[[nodiscard]] std::string_view GetReadableTypeName(const SourceFile*, const semantic::Symbol*);
[[nodiscard]] std::string_view GetReadableTypeName(const semantic::Symbol*);

inline const Scope* FindScope(const Scope* where, const ast::Node* n) {
  if (!where->Container()->Contains(n)) [[unlikely]] {
    return nullptr;
  }

  const Scope* candidate = where;
  while (true) {
    const Scope* narrowed = nullptr;
    for (const auto* child : candidate->GetChildren()) {
      if (child->Container()->Contains(n)) {
        narrowed = child;
        break;
      }
    }

    if (narrowed) {
      candidate = narrowed;
      continue;
    }

    return candidate;
  }
}

}  // namespace utils
}  // namespace vanadium::core::semantic
