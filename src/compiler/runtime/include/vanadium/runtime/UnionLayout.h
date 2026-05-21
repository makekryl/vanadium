#pragma once

#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/rt_reflect.h"

namespace vanadium::rt {

struct GenericUnion {
  vrt_union_active_idx_t active_idx;

  static constexpr auto kUnboundActiveIdx = vrt_union_active_idx_t(-1);
};

inline vrt_union_active_idx_t GetValueUnionSelection(const void* p) {
  return static_cast<const GenericUnion*>(p)->active_idx;
}
inline vrt_union_active_idx_t GetUnionSelection(const vrt_val_t& v) {
  if (tpl::IsTemplateType(v.ty)) {
    return GetValueUnionSelection(tpl::AsGenericTemplateType(v.p)->GetPayload());
  }
  return GetValueUnionSelection(v.p);
}

inline void SetValueUnionSelection(void* p, vrt_union_active_idx_t idx) {
  static_cast<GenericUnion*>(p)->active_idx = idx;
}

}  // namespace vanadium::rt
