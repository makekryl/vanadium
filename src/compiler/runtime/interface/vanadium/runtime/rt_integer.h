#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

using vrt_native_int_t = std::int64_t;

struct vrt_int_t {
  vrt_native_int_t value;
  bool is_bound;
};
struct vrt_int_template_t;

inline vrt_int_t vrt_int_wrap(vrt_native_int_t v) {
  return {.value = v, .is_bound = true};
}

extern "C" {
//

extern const vrt_typeinfo_t integer_typeinfo;
extern const vrt_typeinfo_t integer_template_typeinfo;

void vrt_int_ctor(vrt_int_t*);
void vrt_int_dtor(vrt_int_t*);

// TODO: inline in codegen as a special case along with other primitives
void copy_integer(vrt_int_t* dst, vrt_int_t src);

bool vrt_int_eq(vrt_int_t, vrt_int_t);
bool vrt_int_ne(vrt_int_t, vrt_int_t);
//
bool vrt_int_lt(vrt_int_t, vrt_int_t);
bool vrt_int_le(vrt_int_t, vrt_int_t);
bool vrt_int_gt(vrt_int_t, vrt_int_t);
bool vrt_int_ge(vrt_int_t, vrt_int_t);
//
vrt_int_t vrt_int_neg(vrt_int_t);
//
vrt_int_t vrt_int_add(vrt_int_t, vrt_int_t);
vrt_int_t vrt_int_sub(vrt_int_t, vrt_int_t);
vrt_int_t vrt_int_mul(vrt_int_t, vrt_int_t);
vrt_int_t vrt_int_div(vrt_int_t, vrt_int_t);

///

bool vrt_int_template_match(const vrt_int_t*, const vrt_int_template_t*);

//
}

// NOLINTEND(readability-identifier-naming)
