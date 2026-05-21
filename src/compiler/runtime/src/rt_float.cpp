#include "vanadium/runtime/rt_float.h"

#include <cassert>
#include <cstddef>

#include "vanadium/runtime/BuiltinsTemplates.h"
#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

namespace {
inline void AssertIsBound(vrt_float_t& i) {
  rt::Assert(i.is_bound, "accessing an unbound float value");
}
}  // namespace

const vrt_typeinfo_t float_typeinfo{
    .name = "float",
    .kind = vrt_typekind_e::kFloat,

    .size = sizeof(vrt_float_t),
    .alignment = alignof(vrt_float_t),

    .members = nullptr,

    .construct = rt::helpers::void_erased_v<float_ctor>,
    .destruct = rt::helpers::void_erased_v<float_dtor>,
    .copy = rt::helpers::void_erased_v<copy_float>,

    .counterpart = &float_template_typeinfo,
    .tpl_construct_value = nullptr,
};

void float_ctor(vrt_float_t* p) {
  p->is_bound = false;
}
void float_dtor(vrt_float_t* p) {}

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

void copy_float(vrt_float_t* dst, const vrt_float_t* src) {
  *dst = *src;
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

vrt_float_t vrt_float_neg(vrt_float_t a) {
  AssertIsBound(a);
  return vrt_float_wrap(-a.value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
void float_template_ctor(vrt_float_template_t*);
void float_template_dtor(vrt_float_template_t*);
}

const vrt_typeinfo_t float_template_typeinfo{
    .name = float_typeinfo.name,
    .kind = float_typeinfo.kind,

    .size = sizeof(vrt_float_template_t),
    .alignment = alignof(vrt_float_template_t),

    .members = nullptr,

    .construct = rt::helpers::void_erased_v<float_template_ctor>,
    .destruct = rt::helpers::void_erased_v<float_template_dtor>,
    .copy = rt::helpers::void_erased_v<copy_float>,

    .counterpart = &float_typeinfo,
    .tpl_construct_value = rt::helpers::void_erased_v<float_template_ctor>,
};

void float_template_ctor(vrt_float_template_t* p) {
  rt::tpl::Construct(p);
}
void float_template_dtor(vrt_float_template_t* p) {
  switch (p->tsel) {
    case vrt_template_sel_e::kSpecificValue:
    case vrt_template_sel_e::kValueRange:
      break;
    default:
      rt::tpl::Destruct<float_template_dtor>(p);
      break;
  }
}

bool vrt_float_template_match(const vrt_float_t* v, const vrt_float_template_t* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      return v->value == t->val.value;
    case vrt_template_sel_e::kValueRange: {
      bool matches = true;
      if (t->vmin_present) {
        matches = matches && (t->vmin_exclusive ? (t->range.vmin < v->value) : (t->range.vmin <= v->value));
      }
      if (t->vmax_exclusive) {
        matches = matches && (t->vmax_exclusive ? (t->range.vmax > v->value) : (t->range.vmax >= v->value));
      }
      return matches;
    }
    default:
      assert(false);
  }
}
