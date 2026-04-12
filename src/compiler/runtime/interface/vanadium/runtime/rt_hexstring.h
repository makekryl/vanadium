#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

using hexstring_size_t = std::uint32_t;

struct vrt_hexstring_t {
  union {
    struct {
      std::uint8_t* data;
      hexstring_size_t capacity;  // bytes
    } ext;
    std::uint8_t intl[sizeof(ext)];
  } value;
  hexstring_size_t length;  // nibbles
  bool is_bound;
  bool is_ext;
};

extern "C" {
//

extern const vrt_typeinfo_t hexstring_typeinfo;

void vrt_hexstring_ctor(vrt_hexstring_t*);
void vrt_hexstring_dtor(vrt_hexstring_t*);

void copy_hexstring(vrt_hexstring_t* dst, const vrt_hexstring_t* src);

std::uint8_t* vrt_hexstring_get_buf(vrt_hexstring_t*);
const std::uint8_t* vrt_hexstring_get_cbuf(const vrt_hexstring_t*);

void vrt_hexstring_assign(vrt_hexstring_t* dst, const std::uint8_t* src, hexstring_size_t bit_len);
void vrt_hexstring_concat(vrt_hexstring_t* dst, const vrt_hexstring_t*, const vrt_hexstring_t*);
void vrt_hexstring_singular(vrt_hexstring_t* dst, const vrt_hexstring_t*, hexstring_size_t i);

void vrt_hexstring_shift_left(vrt_hexstring_t* dst, const vrt_hexstring_t*, std::int64_t n);
void vrt_hexstring_shift_right(vrt_hexstring_t* dst, const vrt_hexstring_t*, std::int64_t n);
//
void vrt_hexstring_rotate_left(vrt_hexstring_t* dst, const vrt_hexstring_t*, std::int64_t n);
void vrt_hexstring_rotate_right(vrt_hexstring_t* dst, const vrt_hexstring_t*, std::int64_t n);

std::uint8_t vrt_hexstring_at(const vrt_hexstring_t*, hexstring_size_t i);
void vrt_hexstring_set(vrt_hexstring_t*, hexstring_size_t i, bool v);

bool vrt_hexstring_eq(const vrt_hexstring_t*, const vrt_hexstring_t*);
bool vrt_hexstring_ne(const vrt_hexstring_t*, const vrt_hexstring_t*);

//

void vrt_hexstring_not4b(vrt_hexstring_t* dst, const vrt_hexstring_t*);
void vrt_hexstring_and4b(vrt_hexstring_t* dst, const vrt_hexstring_t*, const vrt_hexstring_t*);
void vrt_hexstring_or4b(vrt_hexstring_t* dst, const vrt_hexstring_t*, const vrt_hexstring_t*);
void vrt_hexstring_xor4b(vrt_hexstring_t* dst, const vrt_hexstring_t*, const vrt_hexstring_t*);

//
}

// NOLINTEND(readability-identifier-naming)
