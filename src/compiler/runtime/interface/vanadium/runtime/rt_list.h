#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

using list_size_t = std::uint32_t;

struct vrt_list_t {
  void* data;
  list_size_t length;
  list_size_t capacity;
  const vrt_typeinfo_t* etype;
};

extern "C" {
//

extern const vrt_typeinfo_t list_typeinfo;

void list_ctor(vrt_list_t*);
void list_dtor(vrt_list_t*);

bool vrt_list_isbound(const vrt_list_t*);

void copy_list(vrt_list_t* dst, const vrt_list_t* src);

void vrt_list_concat(vrt_list_t* dst, const vrt_list_t*, const vrt_list_t*);

void vrt_list_rotate_left(vrt_list_t* dst, const vrt_list_t*, std::int64_t n);
void vrt_list_rotate_right(vrt_list_t* dst, const vrt_list_t*, std::int64_t n);

void* vrt_list_at(const vrt_list_t*, list_size_t i);

bool vrt_list_eq(const vrt_list_t*, const vrt_list_t*);
bool vrt_list_ne(const vrt_list_t*, const vrt_list_t*);

//
}

// NOLINTEND(readability-identifier-naming)
