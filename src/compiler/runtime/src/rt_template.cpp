#include "vanadium/runtime/rt_template.h"

#include <cassert>
#include <cstdio>

#include "vanadium/runtime/BuiltinsTemplates.h"
#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_integer.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/runtime.h"

using namespace vanadium;

namespace {
rt::tpl::GenericTemplateType* AsGenericTemplateType(void* p) {
  return static_cast<rt::tpl::GenericTemplateType*>(p);
}

void* ResetTemplate(const vrt_typeinfo_t* td, void* p, vrt_template_sel_e new_tsel) {
  // printf("ResetTemplate(td->name=%s)\n", td->name);
  assert(rt::tpl::IsTemplateType(td));

  td->destruct(p);
  // assuming ctor is trivial (sets tsel to kSpecificValue, vrt_tpl_generic_ctor), we won't call it

  AsGenericTemplateType(p)->tsel = new_tsel;
  return AsGenericTemplateType(p)->GetPayload();
}
}  // namespace

void vrt_tpl_generic_ctor(void* p) {
  static_cast<rt::tpl::GenericTemplateType*>(p)->tsel = vrt_template_sel_e::kUninitialized;
}
void vrt_tpl_generic_dtor(void (*destruct_tpl)(void*), void* p) {
  auto* t = static_cast<rt::tpl::GenericTemplateType*>(p);
  switch (t->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      assert(false);
      break;
    default:
      rt::tpl::Destruct(destruct_tpl, t);
      break;
  }
}

void* vrt_tpl_get_value(void* p) {
  if (AsGenericTemplateType(p)->tsel != vrt_template_sel_e::kSpecificValue) {
    vrt_panic("Accessing field a of a non-specific template");
  }
  return AsGenericTemplateType(p)->GetPayload();
}

void* vrt_tpl_value(const vrt_typeinfo_t* td, void* p) {
  // printf("vrt_tpl_val(td.name=%s, p=%p)\n", td->name, p);
  auto* val = ResetTemplate(td, p, vrt_template_sel_e::kSpecificValue);
  td->tpl_construct_value(val);
  return val;
}

void* vrt_tpl_list(const vrt_typeinfo_t* td, void* p, vrt_valuelist_size_t n, std::size_t* esz,
                   vrt_template_sel_e lkind) {
  switch (lkind) {
    case vrt_template_sel_e::kValueList:
    case vrt_template_sel_e::kComplementedList:
    case vrt_template_sel_e::kConjunctionList:
      break;
    default:
      vrt_panic("invalid lkind");
      return nullptr;
  }

  const auto esize = td->size;

  auto* vlist = static_cast<rt::tpl::ValueList<void>*>(ResetTemplate(td, p, lkind));
  vlist->data = vrt_alloc(n * esize, 8);  // TODO: take alignment from typeinfo
  vlist->length = n;
  vlist->esize = esize;
  for (vrt_valuelist_size_t i = 0; i < n; ++i) {
    // printf("vrt_tpl_list->ctor(%p)\n", static_cast<std::byte*>(vlist->data) + (i * esize));
    td->construct(static_cast<std::byte*>(vlist->data) + (i * esize));
  }

  *esz = td->size;

  return vlist->data;
}

void vrt_tpl_omit(const vrt_typeinfo_t* td, void* p) {
  ResetTemplate(td, p, vrt_template_sel_e::kOmitValue);
}

void vrt_tpl_any(const vrt_typeinfo_t* td, void* p) {
  ResetTemplate(td, p, vrt_template_sel_e::kAnyValue);
}

void vrt_tpl_any_or_omit(const vrt_typeinfo_t* td, void* p) {
  ResetTemplate(td, p, vrt_template_sel_e::kAnyOrOmit);
}

bool vrt_dynmatcher_invoke(const vrt_dynmatcher_t* dm, const void* obj) {
  return dm->match(dm->ctx, obj);
}

void vrt_dynmatcher_free(const vrt_dynmatcher_t* p) {
  vrt_unifree(p->ctx);
}

namespace {
bool vrt_match_intl(const vrt_typeinfo_t* ty, const vrt_val_t* obj, const vrt_val_t* tobj) {
#define X(name)                                                                                  \
  if (ty == &name##_typeinfo) {                                                                  \
    return rt::tpl::Match<vrt_##name##_template_match>((const vrt_##name##_t*)obj->p,            \
                                                       (const vrt_##name##_template_t*)tobj->p); \
  }
#include "vanadium/runtime/BuiltinTypes.inc"
#undef X

  assert(false);
  return false;
}

}  // namespace

void vrt_match(vrt_boolean_t* res, const vrt_val_t* obj, const vrt_val_t* tobj) {
  assert((obj->ty == tobj->ty->counterpart) && (obj->ty->counterpart == tobj->ty));
  *res = vrt_boolean_wrap(vrt_match_intl(obj->ty, obj, tobj));
}
