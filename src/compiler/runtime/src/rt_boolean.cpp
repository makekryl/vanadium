#include "vanadium/runtime/rt_boolean.h"

#include <cassert>

#include "vanadium/runtime/BuiltinsTemplates.h"
#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

namespace {
inline void AssertIsBound(vrt_boolean_t b) {
  rt::Assert(b.is_bound, "accessing an unbound boolean value");
}
}  // namespace

const vrt_typeinfo_t boolean_typeinfo{
    .name = "boolean",
    .kind = vrt_typekind_e::kBoolean,
    .size = sizeof(vrt_boolean_t),

    .members = nullptr,

    .construct = rt::helpers::void_erased_v<boolean_ctor>,
    .destruct = rt::helpers::void_erased_v<boolean_dtor>,
    .copy = rt::helpers::void_erased_v<&copy_boolean>,

    .counterpart = &boolean_template_typeinfo,
    .tpl_construct_value = nullptr,
};

void boolean_ctor(vrt_boolean_t* p) {
  p->is_bound = false;
}
void boolean_dtor(vrt_boolean_t* p) {}

void copy_boolean(vrt_boolean_t* dst, const vrt_boolean_t* src) {
  *dst = *src;
}

#define DEFINE_BINARY_OP_TO_BOOL(name, op)                    \
  bool vrt_boolean_##name(vrt_boolean_t a, vrt_boolean_t b) { \
    AssertIsBound(a);                                         \
    AssertIsBound(b);                                         \
    return a.value op b.value;                                \
  }

DEFINE_BINARY_OP_TO_BOOL(eq, ==);
DEFINE_BINARY_OP_TO_BOOL(ne, !=);

vrt_boolean_t vrt_boolean_not(vrt_boolean_t b) {
  AssertIsBound(b);
  return vrt_boolean_wrap(!b.value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
void boolean_template_ctor(vrt_boolean_template_t*);
void boolean_template_dtor(vrt_boolean_template_t*);
}

const vrt_typeinfo_t boolean_template_typeinfo{
    .name = boolean_typeinfo.name,
    .kind = boolean_typeinfo.kind,
    .size = sizeof(vrt_boolean_template_t),

    .members = nullptr,

    .construct = rt::helpers::void_erased_v<boolean_template_ctor>,
    .destruct = rt::helpers::void_erased_v<boolean_template_dtor>,
    .copy = rt::helpers::void_erased_v<copy_boolean>,

    .counterpart = &boolean_typeinfo,
    .tpl_construct_value = rt::helpers::void_erased_v<boolean_template_ctor>,
};

void boolean_template_ctor(vrt_boolean_template_t* p) {
  rt::tpl::Construct(p);
}
void boolean_template_dtor(vrt_boolean_template_t* p) {
  switch (p->tsel) {
    case vrt_template_sel_e::kSpecificValue:
    case vrt_template_sel_e::kValueRange:
      break;
    default:
      rt::tpl::Destruct<boolean_template_dtor>(p);
      break;
  }
}

bool vrt_boolean_template_match(const vrt_boolean_t* v, const vrt_boolean_template_t* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      return v->value == t->val.value;
    default:
      assert(false);
  }
}
