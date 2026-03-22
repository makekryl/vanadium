#pragma once

struct vrt_typeinfo_t;

extern "C" {
//

void* vrt_new(const vrt_typeinfo_t*);
void vrt_del(const vrt_typeinfo_t*, void*);

void vrt_stackalloc_mark();
void vrt_stackalloc_sweep();
void* vrt_stackalloc_new(const vrt_typeinfo_t*);

//
}
