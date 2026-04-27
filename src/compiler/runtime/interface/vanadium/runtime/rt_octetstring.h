#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

using octetstring_size_t = std::uint32_t;
using octet_t = std::uint8_t;

struct vrt_octetstring_t {
  union {
    struct {
      octet_t* data;
      octetstring_size_t capacity;
    } ext;
    octet_t intl[sizeof(ext)];
  } value;
  octetstring_size_t length;
  bool is_bound;
  bool is_ext;
};
struct vrt_octetstring_template_t;

extern "C" {
//

extern const vrt_typeinfo_t octetstring_typeinfo;
extern const vrt_typeinfo_t octetstring_template_typeinfo;

void vrt_octetstring_ctor(vrt_octetstring_t*);
void vrt_octetstring_dtor(vrt_octetstring_t*);

void copy_octetstring(vrt_octetstring_t* dst, const vrt_octetstring_t* src);

octet_t* vrt_octetstring_get_buf(vrt_octetstring_t*);
const octet_t* vrt_octetstring_get_cbuf(const vrt_octetstring_t*);

void vrt_octetstring_assign(vrt_octetstring_t* dst, const octet_t* src, octetstring_size_t len);
void vrt_octetstring_concat(vrt_octetstring_t* dst, const vrt_octetstring_t*, const vrt_octetstring_t*);
void vrt_octetstring_singular(vrt_octetstring_t* dst, const vrt_octetstring_t*, octetstring_size_t i);

void vrt_octetstring_shift_left(vrt_octetstring_t* dst, const vrt_octetstring_t*, std::int64_t n);
void vrt_octetstring_shift_right(vrt_octetstring_t* dst, const vrt_octetstring_t*, std::int64_t n);
//
void vrt_octetstring_rotate_left(vrt_octetstring_t* dst, const vrt_octetstring_t*, std::int64_t n);
void vrt_octetstring_rotate_right(vrt_octetstring_t* dst, const vrt_octetstring_t*, std::int64_t n);

octet_t vrt_octetstring_at(const vrt_octetstring_t*, octetstring_size_t i);
void vrt_octetstring_set(vrt_octetstring_t*, octetstring_size_t i, octet_t v);

bool vrt_octetstring_eq(const vrt_octetstring_t*, const vrt_octetstring_t*);
bool vrt_octetstring_ne(const vrt_octetstring_t*, const vrt_octetstring_t*);

//

void vrt_octetstring_not4b(vrt_octetstring_t* dst, const vrt_octetstring_t*);
void vrt_octetstring_and4b(vrt_octetstring_t* dst, const vrt_octetstring_t*, const vrt_octetstring_t*);
void vrt_octetstring_or4b(vrt_octetstring_t* dst, const vrt_octetstring_t*, const vrt_octetstring_t*);
void vrt_octetstring_xor4b(vrt_octetstring_t* dst, const vrt_octetstring_t*, const vrt_octetstring_t*);

//
}

// NOLINTEND(readability-identifier-naming)
