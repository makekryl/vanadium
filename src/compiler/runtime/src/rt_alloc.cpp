#include "vanadium/runtime/rt_alloc.h"

#include <cstddef>
#include <cstdlib>

#include "vanadium/runtime/StaticArena.h"
#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/runtime.h"

namespace {
vanadium::runtime::StaticArena<64 * 1024 * 1024> arena;
}

void* vrt_alloc(std::size_t size, std::size_t alignment) {
  return std::aligned_alloc(alignment, size);
}
void* vrt_stackalloc(std::size_t size, std::size_t alignment) {
  return arena.Alloc(size, alignment);
}
void vrt_unifree(void* p) {
  if (!arena.Contains(p)) {
    std::free(p);
  }
}

void* vrt_new(const vrt_typeinfo_t* td) {
  // TODO: write alignment to typeinfo, use it below
  auto* p = vrt_alloc(td->size, 4);
  // printf("vrt_new(td=%s, tpl=%d, size=%zu) -> %p\n", td->name, vanadium::rt::tpl::IsTemplateType(td), td->size, p);
  td->construct(p);
  return p;
}

void vrt_del(const vrt_typeinfo_t* td, void* p) {
  td->destruct(p);
  vrt_unifree(p);
}

void* vrt_stackalloc_new(const vrt_typeinfo_t* td) {
  auto* p = arena.Alloc(td->size, 8);
  td->construct(p);
  return p;
}

void vrt_stackalloc_mark() {
  arena.Mark();
}
void vrt_stackalloc_sweep() {
  arena.Sweep();
}
