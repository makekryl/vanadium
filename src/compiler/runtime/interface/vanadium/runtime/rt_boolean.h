#pragma once

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

struct vrt_bool_t {
  bool value;
  bool is_bound;
};
struct vrt_bool_template_t;

extern "C" {
//

extern const vrt_typeinfo_t boolean_typeinfo;
extern const vrt_typeinfo_t boolean_template_typeinfo;

// TODO: inline in codegen as a special case along with other primitives
void copy_boolean(vrt_bool_t* dst, vrt_bool_t src);

///

bool vrt_bool_template_match(const vrt_bool_t*, const vrt_bool_template_t*);

//
}

// NOLINTEND(readability-identifier-naming)
