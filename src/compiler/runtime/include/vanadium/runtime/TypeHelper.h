#pragma once

#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/rt_reflect.h"

namespace vanadium::rt {

inline bool IsIndirect(const vrt_typeinfo_t* type) {
  if (tpl::IsTemplateType(type)) {
    return true;
  }

  switch (type->kind) {
    case vrt_typekind_e::kRecord:
    case vrt_typekind_e::kSet:
      return true;
    default:
      return false;
  }
}

}  // namespace vanadium::rt
