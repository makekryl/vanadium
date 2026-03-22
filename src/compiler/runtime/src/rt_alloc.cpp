#include "vanadium/runtime/rt_alloc.h"

#include <print>

#include "vanadium/runtime/StaticArena.h"
#include "vanadium/runtime/runtime.h"

namespace {
vanadium::runtime::StaticArena<64 * 1024 * 1024> arena;
}

void* vrt_new(const vrt_typeinfo_t* td) {
  std::println("vrt_allocate({})", td->name);
  std::fflush(stdout);
  auto* p = std::malloc(td->bytes);
  td->construct(p);
  return p;
}

void vrt_del(const vrt_typeinfo_t* td, void* p) {
  std::println("vrt_free({}): {:p}", td->name, p);
  std::fflush(stdout);
  td->destruct(p);
  std::free(p);
}

void* vrt_stackalloc_new(const vrt_typeinfo_t* td) {
  std::println("vrt_stackalloc_new({})", td->name);
  std::fflush(stdout);
  auto* p = arena.Alloc(td->bytes, 8);
  td->construct(p);
  return p;
}

void vrt_stackalloc_mark() {
  arena.Mark();
}
void vrt_stackalloc_sweep() {
  arena.Sweep();
}
