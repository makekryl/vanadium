#pragma once

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

struct vrt_boolean_t {
  bool value;
  bool is_bound;
};
struct vrt_boolean_template_t;

inline vrt_boolean_t vrt_boolean_wrap(bool v) {
  return {.value = v, .is_bound = true};
}

extern "C" {
//

extern const vrt_typeinfo_t boolean_typeinfo;
extern const vrt_typeinfo_t boolean_template_typeinfo;

// TODO: inline in codegen as a special case along with other primitives
void copy_boolean(vrt_boolean_t* dst, vrt_boolean_t src);

bool vrt_boolean_eq(vrt_boolean_t, vrt_boolean_t);
bool vrt_boolean_ne(vrt_boolean_t, vrt_boolean_t);
//
vrt_boolean_t vrt_boolean_not(vrt_boolean_t);

//
}

// NOLINTEND(readability-identifier-naming)
