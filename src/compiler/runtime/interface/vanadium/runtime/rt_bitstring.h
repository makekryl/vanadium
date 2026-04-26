#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

using bitstring_size_t = std::uint32_t;

struct vrt_bitstring_t {
  union {
    struct {
      std::uint8_t* data;
      bitstring_size_t capacity;  // bytes
    } ext;
    std::uint8_t intl[sizeof(ext)];
  } value;
  bitstring_size_t length;  // bits
  bool is_bound;
  bool is_ext;
};
struct vrt_bitstring_template_t;

extern "C" {
//

extern const vrt_typeinfo_t bitstring_typeinfo;
extern const vrt_typeinfo_t bitstring_template_typeinfo;

void vrt_bitstring_ctor(vrt_bitstring_t*);
void vrt_bitstring_dtor(vrt_bitstring_t*);

void copy_bitstring(vrt_bitstring_t* dst, const vrt_bitstring_t* src);

std::uint8_t* vrt_bitstring_get_buf(vrt_bitstring_t*);
const std::uint8_t* vrt_bitstring_get_cbuf(const vrt_bitstring_t*);

void vrt_bitstring_assign(vrt_bitstring_t* dst, const std::uint8_t* src, bitstring_size_t bit_len);
void vrt_bitstring_concat(vrt_bitstring_t* dst, const vrt_bitstring_t*, const vrt_bitstring_t*);
void vrt_bitstring_singular(vrt_bitstring_t* dst, const vrt_bitstring_t*, bitstring_size_t i);

void vrt_bitstring_shift_left(vrt_bitstring_t* dst, const vrt_bitstring_t*, std::int64_t n);
void vrt_bitstring_shift_right(vrt_bitstring_t* dst, const vrt_bitstring_t*, std::int64_t n);
//
void vrt_bitstring_rotate_left(vrt_bitstring_t* dst, const vrt_bitstring_t*, std::int64_t n);
void vrt_bitstring_rotate_right(vrt_bitstring_t* dst, const vrt_bitstring_t*, std::int64_t n);

bool vrt_bitstring_at(const vrt_bitstring_t*, bitstring_size_t i);
void vrt_bitstring_set(vrt_bitstring_t*, bitstring_size_t i, bool v);

bool vrt_bitstring_eq(const vrt_bitstring_t*, const vrt_bitstring_t*);
bool vrt_bitstring_ne(const vrt_bitstring_t*, const vrt_bitstring_t*);

//

void vrt_bitstring_not4b(vrt_bitstring_t* dst, const vrt_bitstring_t*);
void vrt_bitstring_and4b(vrt_bitstring_t* dst, const vrt_bitstring_t*, const vrt_bitstring_t*);
void vrt_bitstring_or4b(vrt_bitstring_t* dst, const vrt_bitstring_t*, const vrt_bitstring_t*);
void vrt_bitstring_xor4b(vrt_bitstring_t* dst, const vrt_bitstring_t*, const vrt_bitstring_t*);

///

bool vrt_bitstring_template_match(const vrt_bitstring_t*, const vrt_bitstring_template_t*);

//
}

// NOLINTEND(readability-identifier-naming)
