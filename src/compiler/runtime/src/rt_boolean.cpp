#include "vanadium/runtime/rt_boolean.h"

#include <cassert>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

namespace {
inline void CheckIsBound(vrt_bool_t b) {
  rt::Assert(b.is_bound, "accessing an unbound boolean value");
}
}  // namespace

const vrt_typeinfo_t boolean_typeinfo{
    .name = "boolean",
    .kind = vrt_typekind_e::kScalar,
    .size = sizeof(vrt_bool_t),

    .members = nullptr,

    .construct = nullptr,
    .destruct = nullptr,
};

void copy_boolean(vrt_bool_t* dst, vrt_bool_t src) {
  *dst = src;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vrt_bool_template_t {
  vrt_template_sel_e tsel;

  union {
    bool val;
    rt::detail::ValueList<vrt_bool_template_t> list;
    rt::detail::Implication<vrt_bool_template_t>* implication;
    vrt_dynmatcher_t dynmatch;
  };
};

extern "C" {
void vrt_bool_template_ctor(vrt_bool_template_t*);
void vrt_bool_template_dtor(vrt_bool_template_t*);
}

const vrt_typeinfo_t integer_template_typeinfo{
    .name = "integer",
    .kind = vrt_typekind_e::kScalar,
    .is_template = true,
    .size = sizeof(vrt_bool_template_t),

    .members = integer_typeinfo.members,

    .construct = vanadium::rt::helpers::VoidErased<vrt_bool_template_ctor>,
    .destruct = vanadium::rt::helpers::VoidErased<vrt_bool_template_dtor>,

    .counterpart = &integer_typeinfo,
};

void vrt_bool_template_ctor(vrt_bool_template_t* p) {}
void vrt_bool_template_dtor(vrt_bool_template_t* p) {
  switch (p->tsel) {
    case vrt_template_sel_e::kSpecificValue:
    case vrt_template_sel_e::kOmitValue:
    case vrt_template_sel_e::kAnyValue:
    case vrt_template_sel_e::kAnyOrOmit:
      break;
    case vrt_template_sel_e::kValueList:
    case vrt_template_sel_e::kComplementedList:
    case vrt_template_sel_e::kConjunctionMatch:
      p->list.Release<vrt_bool_template_dtor>();
      break;
    case vrt_template_sel_e::kImplicationMatch:
      p->implication->Release<vrt_bool_template_dtor>();
      vrt_unifree(p->implication);
      break;
    case vrt_template_sel_e::kDynamicMatch:
      rt::detail::FreeDynamicMatcher(&p->dynmatch);
      break;
    default:
      assert(false);
  }
}

bool vrt_bool_template_match(const vrt_bool_t* v, const vrt_bool_template_t* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      return v->value == t->val;

    case vrt_template_sel_e::kOmitValue:
      return false;

    case vrt_template_sel_e::kAnyValue:
    case vrt_template_sel_e::kAnyOrOmit:
      return true;

    case vrt_template_sel_e::kValueList:
    case vrt_template_sel_e::kComplementedList:
      return t->list.MatchAny<vrt_bool_template_match>(v) && (t->tsel == vrt_template_sel_e::kValueList);

    case vrt_template_sel_e::kConjunctionMatch:
      return t->list.MatchAll<vrt_bool_template_match>(v);

    case vrt_template_sel_e::kImplicationMatch:
      return t->implication->Match<vrt_bool_template_match>(v);

    case vrt_template_sel_e::kDynamicMatch:
      return rt::detail::DynamicMatch(&t->dynmatch, v);

    default:
      assert(false);
      return false;
  }
}
