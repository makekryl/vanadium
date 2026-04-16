#include "vanadium/runtime/rt_float.h"

#include <cassert>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

namespace {
inline void AssertIsBound(vrt_float_t& i) {
  rt::Assert(i.is_bound, "accessing an unbound float value");
}
}  // namespace

const vrt_typeinfo_t float_typeinfo{
    .name = "float",
    .kind = vrt_typekind_e::kScalar,
    .size = sizeof(vrt_float_t),

    .members = nullptr,

    .construct = vanadium::rt::helpers::VoidErased<vrt_float_ctor>,
    .destruct = vanadium::rt::helpers::VoidErased<vrt_float_dtor>,
};

void vrt_float_ctor(vrt_float_t* p) {
  p->is_bound = false;
}
void vrt_float_dtor(vrt_float_t* p) {}

#define DEFINE_BINARY_OP_TO_BOOL(name, op)              \
  bool vrt_float_##name(vrt_float_t a, vrt_float_t b) { \
    AssertIsBound(a);                                   \
    AssertIsBound(b);                                   \
    return a.value op b.value;                          \
  }
#define DEFINE_BINARY_OP_TO_VALUE(name, op)                    \
  vrt_float_t vrt_float_##name(vrt_float_t a, vrt_float_t b) { \
    AssertIsBound(a);                                          \
    AssertIsBound(b);                                          \
    return {                                                   \
        .value = a.value op b.value,                           \
        .is_bound = true,                                      \
    };                                                         \
  }

void copy_float(vrt_float_t* dst, vrt_float_t src) {
  *dst = src;
}

DEFINE_BINARY_OP_TO_BOOL(eq, ==);
DEFINE_BINARY_OP_TO_BOOL(ne, !=);
//
DEFINE_BINARY_OP_TO_BOOL(lt, <);
DEFINE_BINARY_OP_TO_BOOL(le, <=);
DEFINE_BINARY_OP_TO_BOOL(gt, >);
DEFINE_BINARY_OP_TO_BOOL(ge, >=);

DEFINE_BINARY_OP_TO_VALUE(add, +);
DEFINE_BINARY_OP_TO_VALUE(sub, -);
DEFINE_BINARY_OP_TO_VALUE(mul, *);
DEFINE_BINARY_OP_TO_VALUE(div, /);
