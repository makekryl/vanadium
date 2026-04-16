#include "vanadium/runtime/rt_list.h"

#include <algorithm>
#include <cstddef>
#include <format>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

// NOLINTBEGIN(readability-identifier-naming)

namespace {
void* const kUnboundDataPointerValue = (void*)0xBABECAFE;

inline void AssertIsBound(const vrt_list_t* s) {
  rt::Assert(vrt_list_isbound(s), "accessing an unbound list value");
}

void* AllocArray(const vrt_typeinfo_t* ty, list_size_t len) {
  return vrt_alloc(len * ty->size, alignof(std::max_align_t));
}

inline void* ElementAt(vrt_list_t* l, list_size_t i) {
  return static_cast<std::byte*>(l->data) + (i * l->etype->size);
}
inline void* ElementAt(const vrt_list_t* l, list_size_t i) {
  return ElementAt(const_cast<vrt_list_t*>(l), i);
}

// void* Allocate
}  // namespace

const vrt_typeinfo_t list_typeinfo{
    .name = "list",
    .kind = vrt_typekind_e::kList,
    .size = sizeof(vrt_list_t),

    .members = nullptr,

    .construct = rt::helpers::VoidErased<vrt_list_ctor>,
    .destruct = rt::helpers::VoidErased<vrt_list_dtor>,
};

void vrt_list_ctor(vrt_list_t* l) {
  l->data = kUnboundDataPointerValue;
  l->length = 0;
  l->capacity = 0;
  l->etype = nullptr;
}
void vrt_list_dtor(vrt_list_t* l) {
  if (l->length == 0) {
    return;
  }

  for (list_size_t i = 0; i < l->length; ++i) {
    l->etype->destruct(ElementAt(l, i));
  }
  vrt_unifree(l->data);
}

bool vrt_list_isbound(const vrt_list_t* l) {
  return l->data == kUnboundDataPointerValue;
}

void copy_list(vrt_list_t* dst, const vrt_list_t* src) {
  AssertIsBound(src);

  dst->data = AllocArray(src->etype, src->length);
  dst->length = src->length;
  dst->capacity = src->length;
  dst->etype = src->etype;

  for (list_size_t i = 0; i < src->length; ++i) {
    src->etype->copy(ElementAt(dst, i), ElementAt(src, i));
  }
}

void vrt_list_concat(vrt_list_t* dst, const vrt_list_t* a, const vrt_list_t* b) {
  AssertIsBound(a);
  AssertIsBound(b);
}

namespace {
void AssertNoOverflow(const vrt_list_t* s, list_size_t i) {
  rt::Assert(i < s->length,
             "Index overflow when accessing a list element: the index is {}, but the string has "
             "only {} characters",
             i, s->length);
}
}  // namespace

void* vrt_list_at(const vrt_list_t* s, list_size_t i) {
  AssertIsBound(s);
  AssertNoOverflow(s, i);

  return nullptr;
}

namespace {
void vrt_list_rotate_left_impl(const char* srcbuf, char* buf, list_size_t len, std::int64_t n) {
  std::copy_n(srcbuf + n, len - n, buf);
  std::copy_n(srcbuf, n, buf + len - n);
}
void vrt_list_rotate_right_impl(const char* srcbuf, char* buf, list_size_t len, std::int64_t n) {
  std::copy_n(srcbuf + len - n, n, buf);
  std::copy_n(srcbuf, len - n, buf + n);
}
}  // namespace

void vrt_list_rotate_left(vrt_list_t* dst, const vrt_list_t* s, std::int64_t n) {}

void vrt_list_rotate_right(vrt_list_t* dst, const vrt_list_t* s, std::int64_t n) {}

bool vrt_list_eq(const vrt_list_t* lhs, const vrt_list_t* rhs) {
  AssertIsBound(lhs);
  AssertIsBound(rhs);
  return false;
}
bool vrt_list_ne(const vrt_list_t* lhs, const vrt_list_t* rhs) {
  return !vrt_list_eq(lhs, rhs);
}

// NOLINTEND(readability-identifier-naming)
