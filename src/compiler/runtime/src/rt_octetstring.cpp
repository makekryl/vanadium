#include "vanadium/runtime/rt_octetstring.h"

#include <algorithm>
#include <format>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/StringBase.h"
#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

// NOLINTBEGIN(readability-identifier-naming)

namespace {
inline void AssertIsBound(const vrt_octetstring_t* s) {
  rt::Assert(s->is_bound, "accessing an unbound octetstring value");
}
}  // namespace

const vrt_typeinfo_t octetstring_typeinfo{
    .name = "octetstring",
    .kind = vrt_typekind_e::kString,
    .size = sizeof(vrt_octetstring_t),

    .members = nullptr,

    .construct = rt::helpers::VoidErased<vrt_octetstring_ctor>,
    .destruct = rt::helpers::VoidErased<vrt_octetstring_dtor>,
};

void vrt_octetstring_ctor(vrt_octetstring_t* p) {
  rt::detail::str::Construct(p);
}
void vrt_octetstring_dtor(vrt_octetstring_t* p) {
  rt::detail::str::Destruct(p);
}

octet_t* vrt_octetstring_get_buf(vrt_octetstring_t* s) {
  return rt::detail::str::GetBuf(s);
}
const octet_t* vrt_octetstring_get_cbuf(const vrt_octetstring_t* s) {
  return rt::detail::str::GetCBuf(s);
}

void vrt_octetstring_assign(vrt_octetstring_t* dst, const octet_t* src, octetstring_size_t len) {
  rt::detail::str::Assign(dst, src, len);
}

void copy_octetstring(vrt_octetstring_t* dst, const vrt_octetstring_t* src) {
  AssertIsBound(src);
  rt::detail::str::Copy(dst, src);
}

extern "C" {
void vrt_octetstring_init(vrt_octetstring_t* dst, const octet_t* src, octetstring_size_t len) {
  rt::detail::str::InitFrom(dst, src, len);
}
}

void vrt_octetstring_concat(vrt_octetstring_t* dst, const vrt_octetstring_t* a, const vrt_octetstring_t* b) {
  AssertIsBound(a);
  AssertIsBound(b);

  // TODO: optimize "a := a & ..." and "a := ... & a" (append/prepend)

  const auto total_len = a->length + b->length;
  auto tmp = rt::detail::str::MakeDummy<vrt_octetstring_t>(total_len);

  octet_t* dst_buf = rt::detail::str::GetBuf(&tmp);
  dst_buf = std::copy_n(rt::detail::str::GetCBuf(a), a->length, dst_buf);
  dst_buf = std::copy_n(rt::detail::str::GetCBuf(b), b->length, dst_buf);

  //
  *dst = tmp;
}

namespace {
void AssertNoOverflow(const vrt_octetstring_t* s, octetstring_size_t i) {
  rt::Assert(i < s->length,
             "Index overflow when accessing an octetstring element: the index is {}, but the string has "
             "only {} octets",
             i, s->length);
}
}  // namespace

octet_t vrt_octetstring_at(const vrt_octetstring_t* s, octetstring_size_t i) {
  AssertIsBound(s);
  AssertNoOverflow(s, i);

  return rt::detail::str::GetCBuf(s)[i];
}

void vrt_octetstring_set(vrt_octetstring_t* s, octetstring_size_t i, octet_t v) {
  AssertIsBound(s);
  AssertNoOverflow(s, i);

  if (s->length == i) {
    rt::detail::str::Resize<{.preserve = true}>(s, s->length + 1);
  }

  rt::detail::str::GetBuf(s)[i] = v;
}

void vrt_octetstring_singular(vrt_octetstring_t* dst, const vrt_octetstring_t* s, octetstring_size_t i) {
  const octet_t v = vrt_octetstring_at(s, i);

  dst->is_bound = true;
  dst->is_ext = false;

  dst->value.intl[0] = v;
  dst->length = 1;
}

namespace {
void vrt_octetstring_shift_left_impl(const octet_t* srcbuf, octet_t* buf, octetstring_size_t len, std::int64_t n) {
  std::copy_n(srcbuf + n, len - n, buf);
  std::fill_n(buf + len - n, n, 0);
}
void vrt_octetstring_shift_right_impl(const octet_t* srcbuf, octet_t* buf, octetstring_size_t len, std::int64_t n) {
  std::fill_n(buf, n, 0);
  std::copy_n(srcbuf, len - n, buf + n);
}
}  // namespace

void vrt_octetstring_shift_left(vrt_octetstring_t* dst, const vrt_octetstring_t* s, std::int64_t n) {
  rt::detail::str::PerformShift(dst, s, n,  //
                                vrt_octetstring_shift_left_impl, vrt_octetstring_shift_right_impl);
}

void vrt_octetstring_shift_right(vrt_octetstring_t* dst, const vrt_octetstring_t* s, std::int64_t n) {
  rt::detail::str::PerformShift(dst, s, n,  //
                                vrt_octetstring_shift_right_impl, vrt_octetstring_shift_left_impl);
}

namespace {
void vrt_octetstring_rotate_left_impl(const octet_t* srcbuf, octet_t* buf, octetstring_size_t len, std::int64_t n) {
  std::copy_n(srcbuf + n, len - n, buf);
  std::copy_n(srcbuf, n, buf + len - n);
}
void vrt_octetstring_rotate_right_impl(const octet_t* srcbuf, octet_t* buf, octetstring_size_t len, std::int64_t n) {
  std::copy_n(srcbuf + len - n, n, buf);
  std::copy_n(srcbuf, len - n, buf + n);
}
}  // namespace

void vrt_octetstring_rotate_left(vrt_octetstring_t* dst, const vrt_octetstring_t* s, std::int64_t n) {
  rt::detail::str::PerformRotate(dst, s, n,  //
                                 vrt_octetstring_rotate_left_impl, vrt_octetstring_rotate_right_impl);
}

void vrt_octetstring_rotate_right(vrt_octetstring_t* dst, const vrt_octetstring_t* s, std::int64_t n) {
  rt::detail::str::PerformRotate(dst, s, n,  //
                                 vrt_octetstring_rotate_right_impl, vrt_octetstring_rotate_left_impl);
}

bool vrt_octetstring_eq(const vrt_octetstring_t* lhs, const vrt_octetstring_t* rhs) {
  AssertIsBound(lhs);
  AssertIsBound(rhs);
  return rt::detail::str::Equal(lhs, rhs);
}
bool vrt_octetstring_ne(const vrt_octetstring_t* lhs, const vrt_octetstring_t* rhs) {
  return !vrt_octetstring_eq(lhs, rhs);
}

//

void vrt_octetstring_not4b(vrt_octetstring_t* dst, const vrt_octetstring_t* s) {
  AssertIsBound(s);
  rt::detail::str::UnaryOp(dst, s, std::bit_not<octet_t>{});
}

namespace {
void AssertBinaryOpValidity(std::string_view op_name, const vrt_octetstring_t* lhs, const vrt_octetstring_t* rhs) {
  AssertIsBound(lhs);
  AssertIsBound(rhs);

  rt::Assert(lhs->length == rhs->length, "{} operands must have the same length", op_name);
}
}  // namespace

void vrt_octetstring_and4b(vrt_octetstring_t* dst, const vrt_octetstring_t* lhs, const vrt_octetstring_t* rhs) {
  AssertBinaryOpValidity("and4b", lhs, rhs);
  rt::detail::str::BinaryOp(dst, lhs, rhs, std::bit_and<octet_t>{});
}
void vrt_octetstring_or4b(vrt_octetstring_t* dst, const vrt_octetstring_t* lhs, const vrt_octetstring_t* rhs) {
  AssertBinaryOpValidity("or4b", lhs, rhs);
  rt::detail::str::BinaryOp(dst, lhs, rhs, std::bit_or<octet_t>{});
}
void vrt_octetstring_xor4b(vrt_octetstring_t* dst, const vrt_octetstring_t* lhs, const vrt_octetstring_t* rhs) {
  AssertBinaryOpValidity("xor4b", lhs, rhs);
  rt::detail::str::BinaryOp(dst, lhs, rhs, std::bit_xor<octet_t>{});
}

// NOLINTEND(readability-identifier-naming)
