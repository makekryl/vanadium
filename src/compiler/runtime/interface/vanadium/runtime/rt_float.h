#pragma once

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

struct vrt_float_t {
  double value;
  bool is_bound;
};

extern "C" {
//

extern const vrt_typeinfo_t float_typeinfo;

void vrt_float_ctor(vrt_float_t*);
void vrt_float_dtor(vrt_float_t*);

// TODO: inline in codegen as a special case along with other primitives
void copy_float(vrt_float_t* dst, vrt_float_t src);

bool vrt_float_eq(vrt_float_t, vrt_float_t);
bool vrt_float_ne(vrt_float_t, vrt_float_t);
//
bool vrt_float_lt(vrt_float_t, vrt_float_t);
bool vrt_float_le(vrt_float_t, vrt_float_t);
bool vrt_float_gt(vrt_float_t, vrt_float_t);
bool vrt_float_ge(vrt_float_t, vrt_float_t);
//
vrt_float_t vrt_float_add(vrt_float_t, vrt_float_t);
vrt_float_t vrt_float_sub(vrt_float_t, vrt_float_t);
vrt_float_t vrt_float_mul(vrt_float_t, vrt_float_t);
vrt_float_t vrt_float_div(vrt_float_t, vrt_float_t);

//
}

// NOLINTEND(readability-identifier-naming)
