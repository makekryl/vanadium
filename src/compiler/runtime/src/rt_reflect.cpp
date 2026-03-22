#include <vector>

#include "vanadium/runtime/RuntimeInternals.h"
#include "vanadium/runtime/runtime.h"

namespace {
std::vector<const vrt_module_t*> registered_mods;
}

namespace vanadium::rt {
std::span<const vrt_module_t* const> GetModules() {
  return registered_mods;
}
}  // namespace vanadium::rt

void vrt_register_module(const vrt_module_t* m) {
  registered_mods.push_back(m);
}
