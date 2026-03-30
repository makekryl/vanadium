#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

struct vrt_charstring_t {
  union {
    struct {
      char* data;
      std::uint32_t capacity;
    } ext;
    char intl[sizeof(ext)];
  } value;
  std::uint32_t length;
  bool is_bound;
  bool is_ext;
};

extern "C" {
//

extern const vrt_typeinfo_t charstring_typeinfo;

void vrt_charstring_ctor(vrt_charstring_t*);
void vrt_charstring_dtor(vrt_charstring_t*);

void copy_charstring(vrt_charstring_t* dst, const vrt_charstring_t* src);

char* vrt_charstring_get_buf(vrt_charstring_t*);
const char* vrt_charstring_get_cbuf(const vrt_charstring_t*);

void vrt_charstring_assign(vrt_charstring_t* dst, const char* src, std::uint32_t len);
void vrt_charstring_concat(vrt_charstring_t* dst, const vrt_charstring_t*, const vrt_charstring_t*);
void vrt_charstring_singular(vrt_charstring_t* dst, const vrt_charstring_t*, std::uint32_t i);

void vrt_charstring_rotate_left(vrt_charstring_t* dst, const vrt_charstring_t*, std::int64_t n);
void vrt_charstring_rotate_right(vrt_charstring_t* dst, const vrt_charstring_t*, std::int64_t n);

char vrt_charstring_at(const vrt_charstring_t*, std::uint32_t i);
void vrt_charstring_set(vrt_charstring_t*, std::uint32_t i, char v);

bool vrt_charstring_eq(const vrt_charstring_t*, const vrt_charstring_t*);

//
}

// NOLINTEND(readability-identifier-naming)
