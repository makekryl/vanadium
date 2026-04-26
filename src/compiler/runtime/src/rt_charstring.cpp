#include "vanadium/runtime/rt_charstring.h"

#include <algorithm>
#include <format>
#include <string_view>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/StringBase.h"
#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

// NOLINTBEGIN(readability-identifier-naming)

namespace {
inline void AssertIsBound(const vrt_charstring_t* s) {
  rt::Assert(s->is_bound, "accessing an unbound charstring value");
}
}  // namespace

const vrt_typeinfo_t charstring_typeinfo{
    .name = "charstring",
    .kind = vrt_typekind_e::kCharstring,
    .size = sizeof(vrt_charstring_t),

    .members = nullptr,

    .construct = rt::helpers::VoidErased<vrt_charstring_ctor>,
    .destruct = rt::helpers::VoidErased<vrt_charstring_dtor>,
};

void vrt_charstring_ctor(vrt_charstring_t* p) {
  rt::str::Construct(p);
}
void vrt_charstring_dtor(vrt_charstring_t* p) {
  rt::str::Destruct(p);
}

char* vrt_charstring_get_buf(vrt_charstring_t* s) {
  return rt::str::GetBuf(s);
}
const char* vrt_charstring_get_cbuf(const vrt_charstring_t* s) {
  return rt::str::GetCBuf(s);
}

void vrt_charstring_assign(vrt_charstring_t* dst, const char* src, charstring_size_t len) {
  rt::str::Assign(dst, src, len);
}

void copy_charstring(vrt_charstring_t* dst, const vrt_charstring_t* src) {
  AssertIsBound(src);
  rt::str::Copy(dst, src);
}

extern "C" {
void vrt_charstring_init(vrt_charstring_t* dst, const char* src, charstring_size_t len) {
  rt::str::InitFrom(dst, src, len);
}
}

void vrt_charstring_concat(vrt_charstring_t* dst, const vrt_charstring_t* a, const vrt_charstring_t* b) {
  AssertIsBound(a);
  AssertIsBound(b);

  // TODO: optimize "a := a & ..." and "a := ... & a" (append/prepend)

  const auto total_len = a->length + b->length;

  auto tmp = rt::str::MakeDummy<vrt_charstring_t>(total_len);

  char* dst_buf = rt::str::GetBuf(&tmp);
  dst_buf = std::copy_n(rt::str::GetCBuf(a), a->length, dst_buf);
  dst_buf = std::copy_n(rt::str::GetCBuf(b), b->length, dst_buf);

  //
  *dst = tmp;
}

namespace {
void AssertNoOverflow(const vrt_charstring_t* s, charstring_size_t i) {
  rt::Assert(i < s->length,
             "Index overflow when accessing a charstring element: the index is {}, but the string has "
             "only {} characters",
             i, s->length);
}
}  // namespace

char vrt_charstring_at(const vrt_charstring_t* s, charstring_size_t i) {
  AssertIsBound(s);
  AssertNoOverflow(s, i);

  return rt::str::GetCBuf(s)[i];
}

void vrt_charstring_set(vrt_charstring_t* s, charstring_size_t i, char v) {
  AssertIsBound(s);
  AssertNoOverflow(s, i);

  if (s->length == i) {
    rt::str::Resize<{.preserve = true}>(s, s->length + 1);
  }

  rt::str::GetBuf(s)[i] = v;
}

void vrt_charstring_singular(vrt_charstring_t* dst, const vrt_charstring_t* s, charstring_size_t i) {
  const char v = vrt_charstring_at(s, i);

  dst->is_bound = true;
  dst->is_ext = false;

  dst->value.intl[0] = v;
  dst->length = 1;
}

namespace {
void vrt_charstring_rotate_left_impl(const char* srcbuf, char* buf, charstring_size_t len, std::int64_t n) {
  std::copy_n(srcbuf + n, len - n, buf);
  std::copy_n(srcbuf, n, buf + len - n);
}
void vrt_charstring_rotate_right_impl(const char* srcbuf, char* buf, charstring_size_t len, std::int64_t n) {
  std::copy_n(srcbuf + len - n, n, buf);
  std::copy_n(srcbuf, len - n, buf + n);
}
}  // namespace

void vrt_charstring_rotate_left(vrt_charstring_t* dst, const vrt_charstring_t* s, std::int64_t n) {
  rt::str::PerformRotate(dst, s, n,  //
                         vrt_charstring_rotate_left_impl, vrt_charstring_rotate_right_impl);
}

void vrt_charstring_rotate_right(vrt_charstring_t* dst, const vrt_charstring_t* s, std::int64_t n) {
  rt::str::PerformRotate(dst, s, n,  //
                         vrt_charstring_rotate_right_impl, vrt_charstring_rotate_left_impl);
}

bool vrt_charstring_eq(const vrt_charstring_t* lhs, const vrt_charstring_t* rhs) {
  AssertIsBound(lhs);
  AssertIsBound(rhs);
  return rt::str::Equal(lhs, rhs);
}
bool vrt_charstring_ne(const vrt_charstring_t* lhs, const vrt_charstring_t* rhs) {
  return !vrt_charstring_eq(lhs, rhs);
}

// NOLINTEND(readability-identifier-naming)
