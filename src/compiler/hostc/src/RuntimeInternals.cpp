#include "vanadium/hostc/RuntimeInternals.h"

#include <print>
#include <vector>

#include <vanadium/runtime/runtime.h>

namespace {
std::vector<const vrt_module_t*> registered_mods;
}

namespace vanadium::hostc {
std::span<const vrt_module_t* const> GetModules() {
  return registered_mods;
}
}  // namespace vanadium::hostc

void vrt_register_module(const vrt_module_t* m) {
  registered_mods.push_back(m);
}

void vrt_log_write(const char* s) {
  std::println("LOG :: {}", s);
  std::fflush(stdout);
}
