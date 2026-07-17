#pragma once

#include <unordered_set>

#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>

namespace vanadium::ls::aux {
std::unordered_set<const core::semantic::Symbol*> FindImportContributions(const core::ModuleDescriptor* target,
                                                                          const core::ModuleDescriptor* import);
}
