#pragma once

#include <cstdint>

// NOLINTBEGIN(readability-identifier-naming)

enum class vrt_typekind_e : std::uint8_t {
  kScalar,
  kInteger,
  kFloat,
  kBoolean,

  kString,
  kCharstring,
  kOctetstring,
  kBitstring,
  kHexstring,

  kStruct,
  kRecord,
  kSet,

  kList,
  kRecordOf,
  kSetOf,

  kOptionalMember,
};

struct vrt_struct_member_t;

struct vrt_typeinfo_t {
  const char* name;
  vrt_typekind_e kind;
  bool is_template{false};
  std::size_t size;

  const vrt_struct_member_t** members;

  void (*construct)(void*);
  void (*destruct)(void*);
  void (*copy)(void*, const void*);

  const vrt_typeinfo_t* counterpart;
};

using vrt_valuelist_size_t = std::uint8_t;

struct vrt_val_t {
  void* p;
  const vrt_typeinfo_t* ty;
};

struct vrt_struct_member_t {
  const char* name;
  const vrt_typeinfo_t* type;
  // TODO: offset
};

struct vrt_testcase_t {
  const char* name;
  void (*fn)();
  bool parametrized;
};

struct vrt_module_t {
  const char* name;
  vrt_testcase_t** testcases;
  bool has_control_block;
};

extern "C" {
//

void vrt_register_module(const vrt_module_t*);

//
}

// NOLINTEND(readability-identifier-naming)
