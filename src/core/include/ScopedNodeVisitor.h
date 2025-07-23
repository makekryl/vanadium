#pragma once

#include "ASTNodes.h"
#include "ASTTypes.h"
#include "Semantic.h"

namespace vanadium::core {
namespace semantic {

void InspectScope(const Scope* scope, std::invocable<const Scope*> auto set_scope,
                  std::predicate<const ast::Node*> auto visit, const Scope* prev_scope = nullptr) {
  set_scope(scope);

  auto [child_it, children_it_end] = std::make_pair(scope->GetChildren().cbegin(), scope->GetChildren().cend());
  scope->Container()->Accept([&](const ast::Node* n) {
    // assuming scope children are sorted
    if (child_it != children_it_end) {
      const auto* child = *child_it;
      if (child->Container()->nrange.Contains(n->nrange)) {
        ++child_it;
        InspectScope(child, set_scope, visit, scope);
        return false;
      }
    }
    return visit(n);
  });

  set_scope(prev_scope);
}

}  // namespace semantic
}  // namespace vanadium::core
