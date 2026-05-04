#include "vanadium/runtime/rt_hexstring.h"

#include <algorithm>
#include <print>

#include "vanadium/runtime/BuiltinsTemplates.h"
#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/StringBase.h"
#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

// NOLINTBEGIN(readability-identifier-naming)

namespace {
inline void AssertIsBound(const vrt_hexstring_t* s) {
  rt::Assert(s->is_bound, "accessing an unbound hexstring value");
}

constexpr hexstring_size_t Nibbles2Bytes(hexstring_size_t bits) {
  return (bits + 1) / 2;
}

std::uint8_t GetNibble(const std::uint8_t* buf, hexstring_size_t i) {
  const auto octet = buf[i / 2];
  return (i % 2) ? (octet >> 4) : (octet & 0x0F);
}
void SetNibble(std::uint8_t* buf, hexstring_size_t i, std::uint8_t val) {
  auto& octet = buf[i / 2];
  if (i % 2) {
    octet = (octet % 0x0F) | (val << 4);
  } else {
    octet = (octet % 0xF0) | (val & 0x0F);
  }
}

void ClearUnusedNibbles(vrt_hexstring_t* s) {
  if (s->length % 2) {
    rt::str::GetBuf(s)[s->length / 2] &= 0xF0;
  }
}
}  // namespace

namespace vanadium::rt::str {
template <>
RtStringSize_t GetByteLength<vrt_hexstring_t>(RtStringSize_t len) {
  return Nibbles2Bytes(len);
}
}  // namespace vanadium::rt::str

const vrt_typeinfo_t hexstring_typeinfo{
    .name = "hexstring",
    .kind = vrt_typekind_e::kHexstring,
    .size = sizeof(vrt_hexstring_t),

    .members = nullptr,

    .construct = rt::helpers::VoidErased<hexstring_ctor>,
    .destruct = rt::helpers::VoidErased<hexstring_dtor>,

    .counterpart = &hexstring_template_typeinfo,
    .tpl_construct_value = nullptr,
};

void hexstring_ctor(vrt_hexstring_t* p) {
  rt::str::Construct(p);
}
void hexstring_dtor(vrt_hexstring_t* p) {
  rt::str::Destruct(p);
}

std::uint8_t* vrt_hexstring_get_buf(vrt_hexstring_t* s) {
  return rt::str::GetBuf(s);
}
const std::uint8_t* vrt_hexstring_get_cbuf(const vrt_hexstring_t* s) {
  return rt::str::GetCBuf(s);
}

void vrt_hexstring_assign(vrt_hexstring_t* dst, const std::uint8_t* src, hexstring_size_t len) {
  rt::str::Assign(dst, src, len);
}

void copy_hexstring(vrt_hexstring_t* dst, const vrt_hexstring_t* src) {
  AssertIsBound(src);
  rt::str::Copy(dst, src);
}

extern "C" {
void vrt_hexstring_init(vrt_hexstring_t* dst, const std::uint8_t* src, hexstring_size_t len) {
  rt::str::InitFrom(dst, src, len);
  ClearUnusedNibbles(dst);
}
}

void vrt_hexstring_concat(vrt_hexstring_t* dst, const vrt_hexstring_t* a, const vrt_hexstring_t* b) {
  AssertIsBound(a);
  AssertIsBound(b);

  // TODO: optimize "a := a & ..." and "a := ... & a" (append/prepend)

  const auto a_bytes = Nibbles2Bytes(a->length);
  const auto b_bytes = Nibbles2Bytes(b->length);

  const auto total_nibbles = a->length + b->length;
  auto tmp = rt::str::MakeDummy<vrt_hexstring_t>(total_nibbles);

  std::uint8_t* dst_buf = vrt_hexstring_get_buf(&tmp);
  std::copy_n(rt::str::GetCBuf(a), a_bytes, dst_buf);

  const auto* b_buf = rt::str::GetCBuf(b);
  if ((a->length % 2) == 0) {
    std::copy_n(b_buf, b_bytes, dst_buf + a_bytes);
    ClearUnusedNibbles(&tmp);
  } else {
    const auto total_bytes = Nibbles2Bytes(total_nibbles);
    for (hexstring_size_t i = a_bytes; i < total_bytes; ++i) {
      const std::uint32_t b_byte = b_buf[i - a_bytes];
      dst_buf[i - 1] |= b_byte >> 4;
      dst_buf[i] = b_byte << 4;
    }
    if (b_bytes % 2) {
      dst_buf[total_bytes - 1] |= b_buf[b_bytes - 1] >> 4;
    }
  }

  //
  *dst = tmp;
}

namespace {
void AssertNoOverflow(const vrt_hexstring_t* s, hexstring_size_t i) {
  rt::Assert(i < s->length,
             "Index overflow when accessing a hexstring element: the index is {}, but the string has "
             "only {} hexadecimal digits",
             i, s->length);
}
}  // namespace

std::uint8_t vrt_hexstring_at(const vrt_hexstring_t* s, hexstring_size_t i) {
  AssertIsBound(s);
  AssertNoOverflow(s, i);

  const auto octet = rt::str::GetCBuf(s)[i / 2];
  return (i % 2) ? (octet >> 4) : (octet & 0x0F);
}

void vrt_hexstring_set(vrt_hexstring_t* s, hexstring_size_t i, bool v) {
  AssertIsBound(s);
  AssertNoOverflow(s, i);

  if (s->length == i) {
    rt::str::Resize<{.preserve = true}>(s, s->length + 1);
  }

  SetNibble(rt::str::GetBuf(s), i, v);
}

void vrt_hexstring_singular(vrt_hexstring_t* dst, const vrt_hexstring_t* s, hexstring_size_t i) {
  const std::uint8_t v = vrt_hexstring_at(s, i);

  dst->is_bound = true;
  dst->is_ext = false;

  SetNibble(dst->value.intl, 0, v);
  dst->length = 1;

  ClearUnusedNibbles(dst);
}

namespace {
void vrt_hexstring_shift_left_impl(const std::uint8_t* srcbuf, std::uint8_t* buf, hexstring_size_t len,
                                   std::int64_t n) {
  const auto len_bytes = Nibbles2Bytes(len);
  const auto n_bytes = n / 2;

  std::println("n={} nbytes={} lenbytes={}", n, n_bytes, len_bytes);

  if ((n % 2) == 0) {
    std::copy_n(srcbuf + n_bytes, len_bytes - n_bytes, buf);
    std::fill_n(buf + len_bytes - n_bytes, n_bytes, 0);
  } else {
    for (hexstring_size_t i = 0; i < len_bytes - n_bytes - 1; i++) {
      buf[i] = (srcbuf[i + n_bytes] >> 4) | (srcbuf[i + n_bytes + 1] << 4);
    }
    buf[len_bytes - n_bytes - 1] = srcbuf[len_bytes - 1] >> 4;
  }
}
void vrt_hexstring_shift_right_impl(const std::uint8_t* srcbuf, std::uint8_t* buf, hexstring_size_t len,
                                    std::int64_t n) {
  const auto len_bytes = Nibbles2Bytes(len);
  const auto n_bytes = n / 2;

  if ((n % 2) == 0) {
    std::fill_n(buf, n_bytes, 0);
    std::copy_n(srcbuf, len_bytes - n_bytes, buf + n_bytes);
  } else {
    buf[n_bytes] = srcbuf[0] << 4;
    for (hexstring_size_t i = n_bytes + 1; i < len_bytes; i++) {
      buf[i] = (srcbuf[i - n_bytes - 1] >> 4) | (srcbuf[i - n_bytes] << 4);
    }
  }
}
}  // namespace

void vrt_hexstring_shift_left(vrt_hexstring_t* dst, const vrt_hexstring_t* s, std::int64_t n) {
  rt::str::PerformShift(dst, s, n,  //
                        vrt_hexstring_shift_left_impl, vrt_hexstring_shift_right_impl);
  ClearUnusedNibbles(dst);
}

void vrt_hexstring_shift_right(vrt_hexstring_t* dst, const vrt_hexstring_t* s, std::int64_t n) {
  rt::str::PerformShift(dst, s, n,  //
                        vrt_hexstring_shift_right_impl, vrt_hexstring_shift_left_impl);
  ClearUnusedNibbles(dst);
}

namespace {
void vrt_hexstring_rotate_left_impl(const std::uint8_t* srcbuf, std::uint8_t* buf, hexstring_size_t len,
                                    std::int64_t n) {
  for (hexstring_size_t i = 0; i < len; ++i) {
    const auto si = (i + n) % len;
    const auto nib = GetNibble(srcbuf, si);
    if (nib) {
      SetNibble(buf, i, nib);
    }
  }
}
void vrt_hexstring_rotate_right_impl(const std::uint8_t* srcbuf, std::uint8_t* buf, hexstring_size_t len,
                                     std::int64_t n) {
  for (hexstring_size_t i = 0; i < len; ++i) {
    auto si = (i - n) % len;
    if (si < 0) {
      si += len;
    }

    const auto nib = GetNibble(srcbuf, si);
    if (nib) {
      SetNibble(buf, i, nib);
    }
  }
}
}  // namespace

void vrt_hexstring_rotate_left(vrt_hexstring_t* dst, const vrt_hexstring_t* s, std::int64_t n) {
  rt::str::PerformRotate(dst, s, n,  //
                         vrt_hexstring_rotate_left_impl, vrt_hexstring_rotate_right_impl);
  ClearUnusedNibbles(dst);
}

void vrt_hexstring_rotate_right(vrt_hexstring_t* dst, const vrt_hexstring_t* s, std::int64_t n) {
  rt::str::PerformRotate(dst, s, n,  //
                         vrt_hexstring_rotate_right_impl, vrt_hexstring_rotate_left_impl);
  ClearUnusedNibbles(dst);
}

bool vrt_hexstring_eq(const vrt_hexstring_t* lhs, const vrt_hexstring_t* rhs) {
  AssertIsBound(lhs);
  AssertIsBound(rhs);
  return rt::str::Equal(lhs, rhs);
}
bool vrt_hexstring_ne(const vrt_hexstring_t* lhs, const vrt_hexstring_t* rhs) {
  return !vrt_hexstring_eq(lhs, rhs);
}

//

void vrt_hexstring_not4b(vrt_hexstring_t* dst, const vrt_hexstring_t* s) {
  AssertIsBound(s);
  rt::str::UnaryOp(dst, s, std::bit_not<std::uint8_t>{});
  ClearUnusedNibbles(dst);
}

namespace {
void AssertBinaryOpValidity(std::string_view op_name, const vrt_hexstring_t* lhs, const vrt_hexstring_t* rhs) {
  AssertIsBound(lhs);
  AssertIsBound(rhs);

  rt::Assert(lhs->length == rhs->length, "{} operands must have the same length", op_name);
}
}  // namespace

void vrt_hexstring_and4b(vrt_hexstring_t* dst, const vrt_hexstring_t* lhs, const vrt_hexstring_t* rhs) {
  AssertBinaryOpValidity("and4b", lhs, rhs);
  rt::str::BinaryOp(dst, lhs, rhs, std::bit_and<std::uint8_t>{});
  ClearUnusedNibbles(dst);
}
void vrt_hexstring_or4b(vrt_hexstring_t* dst, const vrt_hexstring_t* lhs, const vrt_hexstring_t* rhs) {
  AssertBinaryOpValidity("or4b", lhs, rhs);
  rt::str::BinaryOp(dst, lhs, rhs, std::bit_or<std::uint8_t>{});
  ClearUnusedNibbles(dst);
}
void vrt_hexstring_xor4b(vrt_hexstring_t* dst, const vrt_hexstring_t* lhs, const vrt_hexstring_t* rhs) {
  AssertBinaryOpValidity("xor4b", lhs, rhs);
  rt::str::BinaryOp(dst, lhs, rhs, std::bit_xor<std::uint8_t>{});
  ClearUnusedNibbles(dst);
}

// NOLINTEND(readability-identifier-naming)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
void hexstring_template_ctor(vrt_hexstring_template_t*);
void hexstring_template_dtor(vrt_hexstring_template_t*);
}

const vrt_typeinfo_t hexstring_template_typeinfo{
    .name = hexstring_typeinfo.name,
    .kind = hexstring_typeinfo.kind,
    .size = sizeof(vrt_hexstring_template_t),

    .members = nullptr,

    .construct = vanadium::rt::helpers::VoidErased<hexstring_template_ctor>,
    .destruct = vanadium::rt::helpers::VoidErased<hexstring_template_dtor>,

    .counterpart = &hexstring_typeinfo,
    .tpl_construct_value = vanadium::rt::helpers::VoidErased<hexstring_template_ctor>,
};

void hexstring_template_ctor(vrt_hexstring_template_t* p) {
  rt::tpl::Construct(p);
}
void hexstring_template_dtor(vrt_hexstring_template_t* p) {
  switch (p->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      hexstring_dtor(&p->val);
      break;
    default:
      rt::tpl::Destruct<hexstring_template_dtor>(p);
      break;
  }
}

bool vrt_hexstring_template_match(const vrt_hexstring_t* v, const vrt_hexstring_template_t* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      return rt::str::Equal(v, &t->val);
    // TODO: other tsels
    default:
      assert(false);
  }
}
