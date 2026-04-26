#include "vanadium/runtime/rt_bitstring.h"

#include <algorithm>
#include <cassert>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/StringBase.h"
#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/rt_template.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

// NOLINTBEGIN(readability-identifier-naming)

namespace {
inline void AssertIsBound(const vrt_bitstring_t* s) {
  rt::Assert(s->is_bound, "accessing an unbound bitstring value");
}

constexpr bitstring_size_t Bits2Bytes(bitstring_size_t bits) {
  return (bits + 7) / 8;
}

constexpr bitstring_size_t GetCellIdx(bitstring_size_t i) {
  return i / 8;
}
constexpr std::uint8_t GetBitMask(bitstring_size_t i) {
  return (1 << 7) >> (i % 8);
}
void SetBit(std::uint8_t* cells, bitstring_size_t i, bool val) {
  const auto mask = GetBitMask(i);
  auto& cell = cells[GetCellIdx(i)];
  if (val) {
    cell |= mask;
  } else {
    cell &= ~mask;
  }
}

void ClearUnusedBits(vrt_bitstring_t* s) {
  const bitstring_size_t used_bits_last = s->length % 8;
  if (used_bits_last != 0) {
    const std::uint8_t mask = std::uint8_t(-1) << (8 - used_bits_last);
    rt::str::GetBuf(s)[Bits2Bytes(s->length) - 1] &= mask;
  }
}
}  // namespace

namespace vanadium::rt::str {
template <>
RtStringSize_t GetByteLength<vrt_bitstring_t>(RtStringSize_t len) {
  return Bits2Bytes(len);
}
}  // namespace vanadium::rt::str

const vrt_typeinfo_t bitstring_typeinfo{
    .name = "bitstring",
    .kind = vrt_typekind_e::kString,
    .size = sizeof(vrt_bitstring_t),

    .members = nullptr,

    .construct = rt::helpers::VoidErased<vrt_bitstring_ctor>,
    .destruct = rt::helpers::VoidErased<vrt_bitstring_dtor>,
};

void vrt_bitstring_ctor(vrt_bitstring_t* p) {
  rt::str::Construct(p);
}
void vrt_bitstring_dtor(vrt_bitstring_t* p) {
  rt::str::Destruct(p);
}

std::uint8_t* vrt_bitstring_get_buf(vrt_bitstring_t* s) {
  return rt::str::GetBuf(s);
}
const std::uint8_t* vrt_bitstring_get_cbuf(const vrt_bitstring_t* s) {
  return rt::str::GetCBuf(s);
}

void vrt_bitstring_assign(vrt_bitstring_t* dst, const std::uint8_t* src, bitstring_size_t len) {
  rt::str::Assign(dst, src, len);
}

void copy_bitstring(vrt_bitstring_t* dst, const vrt_bitstring_t* src) {
  AssertIsBound(src);
  rt::str::Copy(dst, src);
}

extern "C" {
void vrt_bitstring_init(vrt_bitstring_t* dst, const std::uint8_t* src, bitstring_size_t len) {
  rt::str::InitFrom(dst, src, len);
  ClearUnusedBits(dst);
}
}

void vrt_bitstring_concat(vrt_bitstring_t* dst, const vrt_bitstring_t* a, const vrt_bitstring_t* b) {
  AssertIsBound(a);
  AssertIsBound(b);

  // TODO: optimize "a := a & ..." and "a := ... & a" (append/prepend)

  const auto a_bytes = Bits2Bytes(a->length);
  const auto b_bytes = Bits2Bytes(b->length);
  const auto a_dangling_bits = a->length % 8;

  const auto total_bits = a->length + b->length;
  auto tmp = rt::str::MakeDummy<vrt_bitstring_t>(total_bits);

  std::uint8_t* dst_buf = vrt_bitstring_get_buf(&tmp);
  std::copy_n(rt::str::GetCBuf(a), a_bytes, dst_buf);

  const auto* b_buf = rt::str::GetCBuf(b);
  if (a_dangling_bits == 0) {
    std::copy_n(b_buf, b_bytes, dst_buf + a_bytes);
  } else {
    const auto total_bytes = Bits2Bytes(total_bits);
    for (bitstring_size_t i = a_bytes; i < total_bytes; ++i) {
      const std::uint32_t b_byte = b_buf[i - a_bytes];
      dst_buf[i - 1] |= b_byte >> a_dangling_bits;
      dst_buf[i] = b_byte << (8 - a_dangling_bits);
    }
    if ((a_bytes + b_bytes) > total_bytes) {
      dst_buf[total_bytes - 1] |= b_buf[b_bytes - 1] << a_dangling_bits;
    }
  }

  ClearUnusedBits(&tmp);

  //
  *dst = tmp;
}

namespace {
void AssertNoOverflow(const vrt_bitstring_t* s, bitstring_size_t i) {
  rt::Assert(i < s->length,
             "Index overflow when accessing a bitstring element: the index is {}, but the string has "
             "only {} bits",
             i, s->length);
}
}  // namespace

bool vrt_bitstring_at(const vrt_bitstring_t* s, bitstring_size_t i) {
  AssertIsBound(s);
  AssertNoOverflow(s, i);

  return bool(rt::str::GetCBuf(s)[GetCellIdx(i)] & GetBitMask(i));
}

void vrt_bitstring_set(vrt_bitstring_t* s, bitstring_size_t i, bool v) {
  AssertIsBound(s);
  AssertNoOverflow(s, i);

  if (s->length == i) {
    rt::str::Resize<{.preserve = true}>(s, s->length + 1);
  }

  SetBit(rt::str::GetBuf(s), i, v);
}

void vrt_bitstring_singular(vrt_bitstring_t* dst, const vrt_bitstring_t* s, bitstring_size_t i) {
  const bool v = vrt_bitstring_at(s, i);

  dst->is_bound = true;
  dst->is_ext = false;

  SetBit(dst->value.intl, 0, v);
  dst->length = 1;

  ClearUnusedBits(dst);
}

namespace {
void vrt_bitstring_shift_left_impl(const std::uint8_t* srcbuf, std::uint8_t* buf, bitstring_size_t len,
                                   std::int64_t n) {
  const auto len_bytes = Bits2Bytes(len);

  const auto n_bytes = n / 8;
  const auto n_bits = n % 8;

  if (n_bits == 0) {
    std::copy_n(srcbuf + n_bytes, len_bytes - n_bytes, buf);
    std::fill_n(buf + len_bytes - n_bytes, n_bytes, 0);
  } else {
    for (bitstring_size_t i = 0; i < len_bytes - n_bytes - 1; i++) {
      buf[i] = (srcbuf[i + n_bytes] >> n_bits) | (srcbuf[i + n_bytes + 1] << (8 - n_bits));
    }
    buf[len_bytes - n_bytes - 1] = srcbuf[len_bytes - 1] >> n_bits;
  }
}
void vrt_bitstring_shift_right_impl(const std::uint8_t* srcbuf, std::uint8_t* buf, bitstring_size_t len,
                                    std::int64_t n) {
  const auto len_bytes = Bits2Bytes(len);

  const auto n_bytes = n / 8;
  const auto n_bits = n % 8;

  if (n_bits == 0) {
    std::fill_n(buf, n_bytes, 0);
    std::copy_n(srcbuf, len_bytes - n_bytes, buf + n_bytes);
  } else {
    buf[n_bytes] = srcbuf[0] << n_bits;
    for (bitstring_size_t i = n_bytes + 1; i < len_bytes; i++) {
      buf[i] = (srcbuf[i - n_bytes - 1] >> (8 - n_bits)) | (srcbuf[i - n_bytes] << n_bits);
    }
  }
}
}  // namespace

void vrt_bitstring_shift_left(vrt_bitstring_t* dst, const vrt_bitstring_t* s, std::int64_t n) {
  rt::str::PerformShift(dst, s, n,  //
                        vrt_bitstring_shift_left_impl, vrt_bitstring_shift_right_impl);
  ClearUnusedBits(dst);
}

void vrt_bitstring_shift_right(vrt_bitstring_t* dst, const vrt_bitstring_t* s, std::int64_t n) {
  rt::str::PerformShift(dst, s, n,  //
                        vrt_bitstring_shift_right_impl, vrt_bitstring_shift_left_impl);
  ClearUnusedBits(dst);
}

namespace {
void vrt_bitstring_rotate_left_impl(const std::uint8_t* srcbuf, std::uint8_t* buf, bitstring_size_t len,
                                    std::int64_t n) {
  for (bitstring_size_t i = 0; i < len; ++i) {
    const auto si = (i + n) % len;
    const auto byte = srcbuf[si / 8];
    const auto bit = (byte >> (7 - (si & 7))) & 1;
    if (bit) {
      buf[i / 8] |= std::uint8_t(1 << (7 - (i & 7)));
    }
  }
}
void vrt_bitstring_rotate_right_impl(const std::uint8_t* srcbuf, std::uint8_t* buf, bitstring_size_t len,
                                     std::int64_t n) {
  for (bitstring_size_t i = 0; i < len; ++i) {
    auto si = (i - n) % len;
    if (si < 0) {
      si += len;
    }

    const auto byte = srcbuf[si / 8];
    const auto bit = (byte >> (7 - (si & 7))) & 1;
    if (bit) {
      buf[i / 8] |= std::uint8_t(1 << (7 - (i & 7)));
    }
  }
}
}  // namespace

void vrt_bitstring_rotate_left(vrt_bitstring_t* dst, const vrt_bitstring_t* s, std::int64_t n) {
  rt::str::PerformRotate(dst, s, n,  //
                         vrt_bitstring_rotate_left_impl, vrt_bitstring_rotate_right_impl);
  ClearUnusedBits(dst);
}

void vrt_bitstring_rotate_right(vrt_bitstring_t* dst, const vrt_bitstring_t* s, std::int64_t n) {
  rt::str::PerformRotate(dst, s, n,  //
                         vrt_bitstring_rotate_right_impl, vrt_bitstring_rotate_left_impl);
  ClearUnusedBits(dst);
}

bool vrt_bitstring_eq(const vrt_bitstring_t* lhs, const vrt_bitstring_t* rhs) {
  AssertIsBound(lhs);
  AssertIsBound(rhs);
  return rt::str::Equal(lhs, rhs);
}
bool vrt_bitstring_ne(const vrt_bitstring_t* lhs, const vrt_bitstring_t* rhs) {
  return !vrt_bitstring_eq(lhs, rhs);
}

//

void vrt_bitstring_not4b(vrt_bitstring_t* dst, const vrt_bitstring_t* s) {
  AssertIsBound(s);
  rt::str::UnaryOp(dst, s, std::bit_not<std::uint8_t>{});
  ClearUnusedBits(dst);
}

namespace {
void AssertBinaryOpValidity(std::string_view op_name, const vrt_bitstring_t* lhs, const vrt_bitstring_t* rhs) {
  AssertIsBound(lhs);
  AssertIsBound(rhs);

  rt::Assert(lhs->length == rhs->length, "{} operands must have the same length", op_name);
}
}  // namespace

void vrt_bitstring_and4b(vrt_bitstring_t* dst, const vrt_bitstring_t* lhs, const vrt_bitstring_t* rhs) {
  AssertBinaryOpValidity("and4b", lhs, rhs);
  rt::str::BinaryOp(dst, lhs, rhs, std::bit_and<std::uint8_t>{});
  ClearUnusedBits(dst);
}
void vrt_bitstring_or4b(vrt_bitstring_t* dst, const vrt_bitstring_t* lhs, const vrt_bitstring_t* rhs) {
  AssertBinaryOpValidity("or4b", lhs, rhs);
  rt::str::BinaryOp(dst, lhs, rhs, std::bit_or<std::uint8_t>{});
  ClearUnusedBits(dst);
}
void vrt_bitstring_xor4b(vrt_bitstring_t* dst, const vrt_bitstring_t* lhs, const vrt_bitstring_t* rhs) {
  AssertBinaryOpValidity("xor4b", lhs, rhs);
  rt::str::BinaryOp(dst, lhs, rhs, std::bit_xor<std::uint8_t>{});
  ClearUnusedBits(dst);
}

// NOLINTEND(readability-identifier-naming)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vrt_bitstring_template_t {
  vrt_template_sel_e tsel;

  union {
    vrt_bitstring_t val;
    rt::tpl::ValueList<vrt_bitstring_template_t> list;
    rt::tpl::Implication<vrt_bitstring_template_t>* implication;
    vrt_dynmatcher_t dynmatch;
  };
};

extern "C" {
void vrt_bitstring_template_ctor(vrt_bitstring_template_t*);
void vrt_bitstring_template_dtor(vrt_bitstring_template_t*);
}

const vrt_typeinfo_t bitstring_template_typeinfo{
    .name = bitstring_typeinfo.name,
    .kind = bitstring_typeinfo.kind,
    .is_template = true,
    .size = sizeof(vrt_bitstring_template_t),

    .members = bitstring_typeinfo.members,

    .construct = vanadium::rt::helpers::VoidErased<vrt_bitstring_template_ctor>,
    .destruct = vanadium::rt::helpers::VoidErased<vrt_bitstring_template_dtor>,

    .counterpart = &bitstring_typeinfo,
};

void vrt_bitstring_template_ctor(vrt_bitstring_template_t* p) {
  rt::tpl::Construct(p);
}
void vrt_bitstring_template_dtor(vrt_bitstring_template_t* p) {
  switch (p->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      vrt_bitstring_dtor(&p->val);
      break;
    default:
      rt::tpl::Destruct<vrt_bitstring_template_dtor>(p);
      break;
  }
}

bool vrt_bitstring_template_match(const vrt_bitstring_t* v, const vrt_bitstring_template_t* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      return rt::str::Equal(v, &t->val);
    // TODO: other tsels
    default:
      return rt::tpl::Match<vrt_bitstring_template_match>(v, t);
  }
}
