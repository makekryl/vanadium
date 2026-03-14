#include <print>
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

void* vrt_allocate(const vrt_typeinfo_t* td) {
  std::println("vrt_allocate({})", td->name);
  std::fflush(stdout);
  auto* p = malloc(td->bytes);
  td->construct(p);
  return p;
}

void vrt_free(const vrt_typeinfo_t* td, void* p) {
  std::println("vrt_free({}): {:p}", td->name, p);
  std::fflush(stdout);
  td->destruct(p);
  free(p);
}
