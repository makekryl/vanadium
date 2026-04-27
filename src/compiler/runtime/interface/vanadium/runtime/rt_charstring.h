#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

using charstring_size_t = std::uint32_t;

struct vrt_charstring_t {
  union {
    struct {
      char* data;
      charstring_size_t capacity;
    } ext;
    char intl[sizeof(ext)];
  } value;
  charstring_size_t length;
  bool is_bound;
  bool is_ext;
};
struct vrt_charstring_template_t;

extern "C" {
//

extern const vrt_typeinfo_t charstring_typeinfo;
extern const vrt_typeinfo_t charstring_template_typeinfo;

void vrt_charstring_ctor(vrt_charstring_t*);
void vrt_charstring_dtor(vrt_charstring_t*);

void copy_charstring(vrt_charstring_t* dst, const vrt_charstring_t* src);

char* vrt_charstring_get_buf(vrt_charstring_t*);
const char* vrt_charstring_get_cbuf(const vrt_charstring_t*);

void vrt_charstring_assign(vrt_charstring_t* dst, const char* src, charstring_size_t len);
void vrt_charstring_concat(vrt_charstring_t* dst, const vrt_charstring_t*, const vrt_charstring_t*);
void vrt_charstring_singular(vrt_charstring_t* dst, const vrt_charstring_t*, charstring_size_t i);

void vrt_charstring_rotate_left(vrt_charstring_t* dst, const vrt_charstring_t*, std::int64_t n);
void vrt_charstring_rotate_right(vrt_charstring_t* dst, const vrt_charstring_t*, std::int64_t n);

char vrt_charstring_at(const vrt_charstring_t*, charstring_size_t i);
void vrt_charstring_set(vrt_charstring_t*, charstring_size_t i, char v);

bool vrt_charstring_eq(const vrt_charstring_t*, const vrt_charstring_t*);
bool vrt_charstring_ne(const vrt_charstring_t*, const vrt_charstring_t*);

//
}

// NOLINTEND(readability-identifier-naming)
