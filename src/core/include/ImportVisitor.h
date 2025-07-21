#pragma once

#include "Program.h"

namespace vanadium::core::semantic {

struct ImportVisitorOptions {
  bool accept_private_imports;
};

/**
 * @param f(imported_module, via)
 */
template <ImportVisitorOptions Options>
bool VisitImports(Program* program, const ModuleDescriptor& module, auto on_incomplete,
                  std::predicate<ModuleDescriptor*, ModuleDescriptor*> auto f, ModuleDescriptor* via = nullptr) {
  bool incomplete = false;
  const auto report_incomplete = [&] {
    if (via != nullptr && !incomplete) {
      on_incomplete(via);
      incomplete = true;
    }
  };

  for (const auto& [import, descriptor] : module.imports) {
    if ((!Options.accept_private_imports && !descriptor.is_public) || descriptor.transit) {
      continue;
    }

    auto* imported_module = program->GetModule(import);
    if (!imported_module) {
      report_incomplete();
      continue;
    }

    if (!f(imported_module, via)) {
      return false;
    }
  }

  for (const auto& [import, descriptor] : module.imports) {
    if ((!Options.accept_private_imports && !descriptor.is_public) || !descriptor.transit) {
      continue;
    }
    auto* imported_module = program->GetModule(import);
    if (!imported_module) {
      report_incomplete();
      continue;
    }

    if (!VisitImports<{.accept_private_imports = false}>(program, *imported_module, on_incomplete, f,
                                                         via ? via : imported_module)) {
      return false;
    }
  }

  return true;
}

}  // namespace vanadium::core::semantic
