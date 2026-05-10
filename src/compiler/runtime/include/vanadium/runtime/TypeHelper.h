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

// TODO: optimize by splitting up to GetMemberPtr, GetValueMemberPtr, GetTemplateMemberPtr -- diffs in IsIndirect checks
inline void* GetMemberPtr(const vrt_struct_member_t& m, void* p) {
  // printf(" ** GetMemberPtr(%s %s, %p + %zu)\n", m.type->name, m.name, p, m.offset);
  void* x = static_cast<std::byte*>(p) + m.offset;
  if (IsIndirect(m.type)) {
    // printf("   @ INDIRECT\n");
    x = *(void**)x;
  }
  return x;
}

}  // namespace vanadium::rt
