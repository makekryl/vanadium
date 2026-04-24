#include "vanadium/runtime/rt_float.h"

#include <cassert>

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

vrt_float_t vrt_float_neg(vrt_float_t a) {
  AssertIsBound(a);
  return vrt_float_wrap(-a.value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vrt_float_template_t {
  vrt_template_sel_e tsel;

  //
  // moved out of the inner struct to avoid extra padding, saving up 8 bytes
  bool vmin_exclusive;
  bool vmax_exclusive;
  bool vmin_present;
  bool vmax_present;
  //

  union {
    vrt_native_int_t val;
    rt::detail::ValueList<vrt_float_template_t> list;
    struct {
      vrt_native_int_t vmin;
      vrt_native_int_t vmax;
    } range;
    rt::detail::Implication<vrt_float_template_t>* implication;
    vrt_dynmatcher_t dynmatch;
  };
};

extern "C" {
void vrt_float_template_ctor(vrt_float_template_t*);
void vrt_float_template_dtor(vrt_float_template_t*);
}

const vrt_typeinfo_t integer_template_typeinfo{
    .name = "integer",
    .kind = vrt_typekind_e::kScalar,
    .is_template = true,
    .size = sizeof(vrt_float_template_t),

    .members = integer_typeinfo.members,

    .construct = vanadium::rt::helpers::VoidErased<vrt_float_template_ctor>,
    .destruct = vanadium::rt::helpers::VoidErased<vrt_float_template_dtor>,

    .counterpart = &integer_typeinfo,
};

void vrt_float_template_ctor(vrt_float_template_t* p) {}
void vrt_float_template_dtor(vrt_float_template_t* p) {
  switch (p->tsel) {
    case vrt_template_sel_e::kSpecificValue:
    case vrt_template_sel_e::kOmitValue:
    case vrt_template_sel_e::kAnyValue:
    case vrt_template_sel_e::kAnyOrOmit:
    case vrt_template_sel_e::kValueRange:
      break;
    case vrt_template_sel_e::kValueList:
    case vrt_template_sel_e::kComplementedList:
    case vrt_template_sel_e::kConjunctionMatch:
      p->list.Release<vrt_float_template_dtor>();
      break;
    case vrt_template_sel_e::kImplicationMatch:
      p->implication->Release<vrt_float_template_dtor>();
      vrt_unifree(p->implication);
      break;
    case vrt_template_sel_e::kDynamicMatch:
      rt::detail::FreeDynamicMatcher(&p->dynmatch);
      break;
    default:
      assert(false);
  }
}

bool vrt_float_template_match(const vrt_float_t* v, const vrt_float_template_t* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      return v->value == t->val;

    case vrt_template_sel_e::kOmitValue:
      return false;

    case vrt_template_sel_e::kAnyValue:
    case vrt_template_sel_e::kAnyOrOmit:
      return true;

    case vrt_template_sel_e::kValueRange: {
      // TODO: -+infinity
      bool matches = true;
      if (t->vmin_present) {
        matches = matches && (t->vmin_exclusive ? (t->range.vmin < v->value) : (t->range.vmin <= v->value));
      }
      if (t->vmax_exclusive) {
        matches = matches && (t->vmax_exclusive ? (t->range.vmax > v->value) : (t->range.vmax >= v->value));
      }
      return matches;
    }

    case vrt_template_sel_e::kValueList:
    case vrt_template_sel_e::kComplementedList:
      return t->list.MatchAny<vrt_float_template_match>(v) && (t->tsel == vrt_template_sel_e::kValueList);

    case vrt_template_sel_e::kConjunctionMatch:
      return t->list.MatchAll<vrt_float_template_match>(v);

    case vrt_template_sel_e::kImplicationMatch:
      return t->implication->Match<vrt_float_template_match>(v);

    case vrt_template_sel_e::kDynamicMatch:
      return rt::detail::DynamicMatch(&t->dynmatch, v);

    default:
      assert(false);
      return false;
  }
}
