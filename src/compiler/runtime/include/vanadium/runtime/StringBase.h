#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <type_traits>

#include "vanadium/runtime/rt_alloc.h"

namespace vanadium::rt::str {

using RtStringSize_t = std::uint32_t;

template <typename T>
concept RtString = requires(T t) {
  requires std::is_pointer_v<decltype(t.value.ext.data)>;
  { auto(t.value.ext.capacity) } -> std::same_as<RtStringSize_t>;
  //
  requires std::is_array_v<decltype(t.value.intl)>;

  requires std::same_as<std::remove_pointer_t<decltype(t.value.ext.data)>,
                        std::remove_extent_t<decltype(t.value.intl)>>;

  { auto(t.length) } -> std::same_as<RtStringSize_t>;
  { auto(t.is_bound) } -> std::same_as<bool>;
  { auto(t.is_ext) } -> std::same_as<bool>;
};

template <RtString S>
using Element = std::remove_pointer_t<decltype(std::declval<S>().value.ext.data)>;

template <RtString S>
constexpr std::size_t kSmallCapacity = std::extent_v<decltype(std::declval<S>().value.intl)>;

template <RtString S>
Element<S>* AllocBuf(RtStringSize_t len) {
  return static_cast<Element<S>*>(vrt_alloc(len, 1));
}

template <RtString S>
RtStringSize_t GetByteLength(RtStringSize_t len) {
  return len;
}

struct LengthUpdateOpts {
  bool preserve{false};
};

template <LengthUpdateOpts Opts = {}>
void Resize(RtString auto* s, RtStringSize_t len) {
  using S = std::remove_pointer_t<decltype(s)>;

  const auto byte_len = GetByteLength<S>(len);
  const auto current_byte_len = GetByteLength<S>(s->length);

  if (byte_len <= kSmallCapacity<S>) {
    if (s->is_ext) {
      auto* ext_buf = s->value.ext.data;
      if constexpr (Opts.preserve) {
        std::copy_n(ext_buf, current_byte_len, s->value.intl);
      }
      vrt_unifree(ext_buf);
      s->is_ext = false;
    }
  } else {
    if (!s->is_ext) {
      auto* ext_buf = AllocBuf<S>(byte_len);
      if constexpr (Opts.preserve) {
        std::copy_n(s->value.intl, current_byte_len, ext_buf);
      }
      s->value.ext.data = ext_buf;
      s->value.ext.capacity = len;
      s->is_ext = true;
    } else if (byte_len > s->value.ext.capacity) {
      if constexpr (Opts.preserve) {
        const auto newcap = std::max(byte_len, current_byte_len * 2);
        auto* new_buf = AllocBuf<S>(newcap);
        std::copy_n(s->value.ext.data, current_byte_len, new_buf);
        vrt_unifree(s->value.ext.data);
        s->value.ext.capacity = newcap;
      } else {
        vrt_unifree(s->value.ext.data);
        s->value.ext.data = AllocBuf<S>(byte_len);
        s->value.ext.capacity = byte_len;
      }
    }
  }
  s->length = len;
}

template <RtString S>
S MakeDummy(RtStringSize_t len) {
  S r;
  r.is_bound = true;
  r.is_ext = false;
  Resize(&r, len);
  return r;
}

template <RtString S>
void Construct(S* p) {
  p->is_bound = false;
  p->is_ext = false;  // that's why not "is_sso" - if we just zero memory, is_sso would be false
  p->length = 0;
}
template <RtString S>
void Destruct(S* p) {
  if (p->is_bound && p->is_ext) {
    vrt_unifree(p->value.ext.data);
  }
}

template <RtString S>
Element<S>* GetBuf(S* s) {
  return s->is_ext ? s->value.ext.data : s->value.intl;
}
template <RtString S>
const Element<S>* GetCBuf(const S* s) {
  return GetBuf(const_cast<S*>(s));
}

template <RtString S>
void Assign(S* dst, const Element<S>* src, RtStringSize_t len) {
  Resize(dst, len);
  std::copy_n(src, GetByteLength<S>(len), GetBuf(dst));
}

template <RtString S>
void Copy(S* dst, const S* src) {
  dst->is_bound = true;
  Assign(dst, GetCBuf(src), src->length);
}

template <RtString S>
void InitFrom(S* dst, const Element<S>* src, RtStringSize_t len) {
  dst->is_ext = false;
  dst->is_bound = true;
  Assign(dst, src, len);
}

//

template <RtString S>
bool Equal(const S* lhs, const S* rhs) {
  return (lhs->length == rhs->length) &&
         std::equal(GetCBuf(lhs), GetCBuf(lhs) + GetByteLength<S>(lhs->length), GetCBuf(rhs));
}

//

template <RtString S>
void PerformGenericShiftOperation(S* dst, const S* s, std::int64_t n, auto f) {
  const auto len = s->length;

  if (len == 0 || n == 0) {
    S tmp;
    Copy(&tmp, s);
    *dst = tmp;
    return;
  }

  auto tmp = MakeDummy<S>(len);
  auto* buf = GetBuf(&tmp);

  f(buf, len);

  *dst = tmp;
}

template <RtString S>
void PerformShift(S* dst, const S* s, std::int64_t n, auto do_shift, auto do_shift_inv) {
  PerformGenericShiftOperation(dst, s, n, [&](auto* buf, auto len) {
    const auto* srcbuf = GetCBuf(s);
    if (n < 0) {
      do_shift_inv(srcbuf, buf, len, std::min<std::int64_t>(-n, len));
    } else {
      do_shift(srcbuf, buf, len, std::min<std::int64_t>(n, len));
    }
  });
}

template <RtString S>
void PerformRotate(S* dst, const S* s, std::int64_t n, auto do_rotate, auto do_rotate_inv) {
  PerformGenericShiftOperation(dst, s, n, [&](auto* buf, auto len) {
    const auto* srcbuf = GetCBuf(s);
    if (n < 0) {
      do_rotate_inv(srcbuf, buf, len, (-n) % len);
    } else {
      do_rotate(srcbuf, buf, len, n % len);
    }
  });
}

//

template <RtString S, typename F>
  requires requires(F f) {
    { f(std::declval<Element<S>>()) } -> std::same_as<Element<S>>;
  }
void UnaryOp(S* dst, const S* s, F apply) {
  const auto len = s->length;

  auto tmp = MakeDummy<S>(len);

  auto* buf = GetBuf(&tmp);
  const auto* srcbuf = GetCBuf(s);

  for (RtStringSize_t i = 0; i < len; ++i) {
    buf[i] = apply(srcbuf[i]);
  }

  *dst = tmp;
}

template <RtString S, typename F>
  requires requires(F f) {
    { f(std::declval<Element<S>>(), std::declval<Element<S>>()) } -> std::same_as<Element<S>>;
  }
void BinaryOp(S* dst, const S* lhs, const S* rhs, F apply) {
  const auto len = lhs->length;

  auto tmp = MakeDummy<S>(len);

  auto* buf = GetBuf(&tmp);
  const auto* lbuf = GetCBuf(lhs);
  const auto* rbuf = GetCBuf(rhs);

  for (RtStringSize_t i = 0; i < len; ++i) {
    buf[i] = apply(lbuf[i], rbuf[i]);
  }

  *dst = tmp;
}

}  // namespace vanadium::rt::str
