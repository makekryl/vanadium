#pragma once

#include <vanadium/lib/Metaprogramming.h>

#include "Program.h"

namespace vanadium::core::semantic {

struct ImportVisitorOptions {
  bool accept_private_imports;
};

/**
 * @param f(imported_module, via)
 */
template <ImportVisitorOptions Options>
bool VisitImports(Program* program, const ModuleDescriptor& module,
                  mp::Consumer<ModuleDescriptor*, std::string_view> auto on_missing,
                  std::predicate<ModuleDescriptor*, ModuleDescriptor*> auto f, ModuleDescriptor* via = nullptr) {
  const auto report_missing = [&](std::string_view missing_module) {
    on_missing(via, missing_module);
  };

  for (const auto& [import, descriptor] : module.imports) {
    if ((!Options.accept_private_imports && !descriptor.is_public) || descriptor.transit) {
      continue;
    }

    auto* imported_module = program->GetModule(import);
    if (!imported_module) {
      report_missing(import);
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
      report_missing(import);
      continue;
    }

    if (!VisitImports<{.accept_private_imports = false}>(program, *imported_module, on_missing, f,
                                                         via ? via : imported_module)) {
      return false;
    }
  }

  return true;
}

}  // namespace vanadium::core::semantic
