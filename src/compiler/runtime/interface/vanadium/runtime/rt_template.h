#pragma once

#include <cstdint>

#include "vanadium/runtime/rt_boolean.h"
#include "vanadium/runtime/rt_reflect.h"

// NOLINTBEGIN(readability-identifier-naming)

enum class vrt_template_sel_e : std::uint8_t {
  kUninitialized = 0,

  kSpecificValue,

  kOmitValue,
  kAnyValue,
  kAnyOrOmit,

  kValueList,
  kComplementedList,
  kConjunctionList,

  kValueRange,
  kStringPattern,

  kSuperset,
  kSubset,

  kImplication,

  kDecode,
  kDynamic,
};

struct vrt_dynmatcher_t {
  void* ctx;
  bool (*match)(const void* ctx, const void* obj);
};

extern "C" {
//

void vrt_tpl_generic_ctor(void* p);
void vrt_tpl_generic_dtor(void (*destruct_tpl)(void*), void* p);

void* vrt_tpl_get_value(void*);
void* vrt_tpl_value(const vrt_typeinfo_t*, void*);
void* vrt_tpl_list(const vrt_typeinfo_t*, void*, vrt_valuelist_size_t n, /* out */ std::size_t* esz,
                   vrt_template_sel_e lkind);
void vrt_tpl_omit(const vrt_typeinfo_t*, void*);
void vrt_tpl_any(const vrt_typeinfo_t*, void*);
void vrt_tpl_any_or_omit(const vrt_typeinfo_t*, void*);

bool vrt_dynmatcher_invoke(const vrt_dynmatcher_t*, const void*);
void vrt_dynmatcher_free(const vrt_dynmatcher_t*);

void vrt_valueof(void* result, const vrt_val_t* obj);
void vrt_match(vrt_boolean_t*, const vrt_val_t* obj, const vrt_val_t* tobj);

//
}

// NOLINTEND
