#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

using vrt_native_int_t = std::int64_t;

struct vrt_integer_t {
  vrt_native_int_t value;
  bool is_bound;
};
struct vrt_integer_template_t;

inline vrt_integer_t vrt_integer_wrap(vrt_native_int_t v) {
  return {.value = v, .is_bound = true};
}

extern "C" {
//

extern const vrt_typeinfo_t integer_typeinfo;
extern const vrt_typeinfo_t integer_template_typeinfo;

void integer_ctor(vrt_integer_t*);
void integer_dtor(vrt_integer_t*);

// TODO: inline in codegen as a special case along with other primitives
void copy_integer(vrt_integer_t* dst, vrt_integer_t src);

bool vrt_integer_eq(vrt_integer_t, vrt_integer_t);
bool vrt_integer_ne(vrt_integer_t, vrt_integer_t);
//
bool vrt_integer_lt(vrt_integer_t, vrt_integer_t);
bool vrt_integer_le(vrt_integer_t, vrt_integer_t);
bool vrt_integer_gt(vrt_integer_t, vrt_integer_t);
bool vrt_integer_ge(vrt_integer_t, vrt_integer_t);
//
vrt_integer_t vrt_integer_neg(vrt_integer_t);
//
vrt_integer_t vrt_integer_add(vrt_integer_t, vrt_integer_t);
vrt_integer_t vrt_integer_sub(vrt_integer_t, vrt_integer_t);
vrt_integer_t vrt_integer_mul(vrt_integer_t, vrt_integer_t);
vrt_integer_t vrt_integer_div(vrt_integer_t, vrt_integer_t);

///

void vrt_tpl_integer_range(vrt_integer_template_t*,  //
                           vrt_native_int_t vmin, bool vmin_excl, vrt_native_int_t vmax, bool vmax_excl);

//
}

// NOLINTEND(readability-identifier-naming)
