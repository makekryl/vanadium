#pragma once

#include <cstddef>

struct vrt_typeinfo_t;

extern "C" {
//

void* vrt_alloc(std::size_t size, std::size_t alignment);
void* vrt_stackalloc(std::size_t size, std::size_t alignment);
void vrt_unifree(void*);

void* vrt_new(const vrt_typeinfo_t*);
void vrt_del(const vrt_typeinfo_t*, void*);

void vrt_stackalloc_mark();
void vrt_stackalloc_sweep();
void* vrt_stackalloc_new(const vrt_typeinfo_t*);

//
}
