#include "vanadium/ls/aux/ImportInspector.h"

#include <unordered_set>

#include <vanadium/ast/ASTNodes.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>
#include <vanadium/core/utils/ScopedNodeVisitor.h>

#include "vanadium/ast/utils/ASTUtils.h"

namespace vanadium::ls::aux {
std::unordered_set<const core::semantic::Symbol*> FindImportContributions(const core::ModuleDescriptor* target,
                                                                          const core::ModuleDescriptor* import) {
  if (!target->dependencies.contains(const_cast<core::ModuleDescriptor*>(import))) {
    // ^ todo: revisit this const_cast
    return {};
  }

  std::unordered_set<const core::semantic::Symbol*> contributions;

  const core::semantic::Scope* scope{nullptr};
  core::semantic::InspectScope(
      target->sf->module->scope,
      [&](const core::semantic::Scope* scope_under_inspection) {
        scope = scope_under_inspection;
      },
      [&](const ast::Node* n) -> bool {
        switch (n->nkind) {
          case ast::NodeKind::Ident: {
            const auto& isym = core::checker::ResolveExprSymbol(target->sf, scope, n->As<ast::nodes::Ident>());
            if (!isym) {
              break;
            }
            const auto* decl = isym->Declaration();
            if (decl && import->sf == ast::utils::SourceFileOf(decl)) {
              contributions.emplace(isym.sym);
            }
            break;
          }
          // TODO: case for CompositeLiteral with optimization
          default:
            break;
        }
        return true;
      });

  return contributions;
}
}  // namespace vanadium::ls::aux
