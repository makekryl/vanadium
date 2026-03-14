#pragma once

// NOLINTBEGIN(readability-identifier-naming)

struct vrt_typeinfo_t;

struct vrt_optional_t {
  void* value;
  const vrt_typeinfo_t* member_type;
};

extern "C" {
//

extern const vrt_typeinfo_t vrt_optional_typeinfo;

void* vrt_optional_get(const vrt_optional_t*);
void vrt_optional_set(vrt_optional_t*, void*);
bool vrt_optional_is_present(const vrt_optional_t*);

//
}

// NOLINTEND(readability-identifier-naming)
