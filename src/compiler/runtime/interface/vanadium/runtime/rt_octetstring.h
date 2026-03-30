#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

struct vrt_octetstring_t {
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

extern const vrt_typeinfo_t octetstring_typeinfo;

void vrt_octetstring_ctor(vrt_octetstring_t*);
void vrt_octetstring_dtor(vrt_octetstring_t*);

void copy_octetstring(vrt_octetstring_t* dst, const vrt_octetstring_t* src);

char* vrt_octetstring_get_buf(vrt_octetstring_t*);
const char* vrt_octetstring_get_cbuf(const vrt_octetstring_t*);

void vrt_octetstring_assign(vrt_octetstring_t* dst, const char* src, std::uint32_t len);
void vrt_octetstring_concat(vrt_octetstring_t* dst, const vrt_octetstring_t*, const vrt_octetstring_t*);
void vrt_octetstring_singular(vrt_octetstring_t* dst, const vrt_octetstring_t*, std::uint32_t i);

char vrt_octetstring_at(const vrt_octetstring_t*, std::uint32_t i);
void vrt_octetstring_set(vrt_octetstring_t*, std::uint32_t i, char v);

bool vrt_octetstring_eq(const vrt_octetstring_t*, const vrt_octetstring_t*);

//
}

// NOLINTEND(readability-identifier-naming)
