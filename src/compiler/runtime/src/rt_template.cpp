#include "vanadium/runtime/rt_template.h"

#include <cassert>
#include <cstdio>

#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/runtime.h"

using namespace vanadium;

namespace {
void* ResetTemplate(const vrt_typeinfo_t* td, void* p, vrt_template_sel_e new_tsel) {
  assert(td->is_template);

  td->destruct(p);
  // assuming ctor is trivial (sets tsel to kSpecificValue), we won't call it

  auto* t = static_cast<rt::tpl::GenericTemplateTypeLayout*>(p);
  t->tsel = new_tsel;

  return t->GetPayload();
}
}  // namespace

void* vrt_tpl_value(const vrt_typeinfo_t* td, void* p) {
  printf("vrt_tpl_value(%p)\n", p);
  auto* val = ResetTemplate(td, p, vrt_template_sel_e::kSpecificValue);
  td->counterpart->construct(val);
  return val;
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

bool vrt_match(const vrt_typeinfo_t* ty, const void* obj, const void* tobj) {
#define X(name)                                                                                           \
  if (ty == &name##_typeinfo) {                                                                           \
    return vrt_##name##_template_match((const vrt_##name##_t*)obj, (const vrt_##name##_template_t*)tobj); \
  }
#include "rt_builtin_types.inc"
#undef X

  assert(false);
  return false;
}
