#pragma once

#include <cstdint>

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
  kConjunction,

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

void* vrt_tpl_value(const vrt_typeinfo_t*, void*);
void vrt_tpl_omit(const vrt_typeinfo_t*, void*);
void vrt_tpl_any(const vrt_typeinfo_t*, void*);
void vrt_tpl_any_or_omit(const vrt_typeinfo_t*, void*);

bool vrt_dynmatcher_invoke(const vrt_dynmatcher_t*, const void*);
void vrt_dynmatcher_free(const vrt_dynmatcher_t*);

bool vrt_match(const vrt_typeinfo_t* ty, const void* obj, const void* tobj);

//
}

// NOLINTEND
