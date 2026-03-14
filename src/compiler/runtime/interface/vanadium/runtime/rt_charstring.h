#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

struct vrt_charstring_t {
  const char* value;
  std::uint32_t length;
  bool is_bound;
};

extern "C" {
//

extern const vrt_typeinfo_t charstring_typeinfo;

// TODO: inline in codegen as a special case along with other primitives
void copy_charstring(vrt_charstring_t* dst, vrt_charstring_t src);

//
}

// NOLINTEND(readability-identifier-naming)
