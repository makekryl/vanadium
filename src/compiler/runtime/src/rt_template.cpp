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
void* ResetTemplate(const vrt_typeinfo_t* td, void* p, vrt_template_sel_e new_tsel) {
  printf("ResetTemplate(td->name=%s)\n", td->name);
  assert(td->is_template);

  td->destruct(p);
  // assuming ctor is trivial (sets tsel to kSpecificValue), we won't call it

  auto* t = static_cast<rt::tpl::GenericTemplateTypeLayout*>(p);
  t->tsel = new_tsel;

  return t->GetPayload();
}
}  // namespace

void* vrt_tpl_value(const vrt_typeinfo_t* td, void* p) {
  auto* val = ResetTemplate(td, p, vrt_template_sel_e::kSpecificValue);
  td->counterpart->construct(val);
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
  for (vrt_valuelist_size_t i = 0; i < n; ++i) {
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
