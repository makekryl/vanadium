#include "vanadium/runtime/rt_integer.h"

#include <cassert>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

namespace {
inline void AssertIsBound(vrt_int_t& i) {
  rt::Assert(i.is_bound, "accessing an unbound integer value");
}
}  // namespace

const vrt_typeinfo_t integer_typeinfo{
    .name = "integer",
    .kind = vrt_typekind_e::kScalar,
    .bytes = sizeof(vrt_int_t),

    .members = nullptr,

    .construct = vanadium::rt::helpers::VoidErased<vrt_int_ctor>,
    .destruct = vanadium::rt::helpers::VoidErased<vrt_int_dtor>,
};

extern "C" {
void vrt_int_dtor_big(vrt_int_t* p) {
  // assert(p->is_bound && p->is_big);
  // TODO: implement big numbers support
}
}

void vrt_int_ctor(vrt_int_t* p) {
  p->is_bound = false;
  // p->is_big = false;
}
void vrt_int_dtor(vrt_int_t* p) {
  // if (p->is_bound && p->is_big) {
  //   vrt_int_dtor_big(p);
  // }
}

#define DEFINE_BINARY_OP_TO_BOOL(name, op)        \
  bool vrt_int_##name(vrt_int_t a, vrt_int_t b) { \
    AssertIsBound(a);                             \
    AssertIsBound(b);                             \
    return a.value op b.value;                    \
  }
#define DEFINE_BINARY_OP_TO_VALUE(name, op)            \
  vrt_int_t vrt_int_##name(vrt_int_t a, vrt_int_t b) { \
    AssertIsBound(a);                                  \
    AssertIsBound(b);                                  \
    return {                                           \
        .value = a.value op b.value,                   \
        .is_bound = true,                              \
    };                                                 \
  }

void copy_integer(vrt_int_t* dst, vrt_int_t src) {
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
