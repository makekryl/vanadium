#include "vanadium/runtime/rt_octetstring.h"

#include <algorithm>
#include <concepts>
#include <format>
#include <utility>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

// NOLINTBEGIN(readability-identifier-naming)

namespace {
constexpr octetstring_size_t kSmallCapacity =
    sizeof(std::declval<vrt_octetstring_t>().value.intl);  // is not zero-terminated

inline void AssertIsBound(const vrt_octetstring_t* s) {
  rt::Assert(s->is_bound, "accessing an unbound octetstring value");
}

octet_t* vrt_alloc_charbuf(octetstring_size_t len) {
  return static_cast<octet_t*>(vrt_alloc(len, 1));
}

struct LengthUpdateOpts {
  bool preserve{true};
};

template <LengthUpdateOpts Opts = {}>
void octetstring_update_length(vrt_octetstring_t* s, octetstring_size_t len) {
  if (len <= kSmallCapacity) {
    if (s->is_ext) {
      auto* ext_buf = s->value.ext.data;
      if constexpr (Opts.preserve) {
        std::copy_n(ext_buf, s->length, s->value.intl);
      }
      vrt_unifree(ext_buf);
      s->is_ext = false;
    }
  } else {
    if (!s->is_ext) {
      auto* ext_buf = vrt_alloc_charbuf(len);
      if constexpr (Opts.preserve) {
        std::copy_n(s->value.intl, s->length, ext_buf);
      }
      s->value.ext.data = ext_buf;
      s->value.ext.capacity = len;
      s->is_ext = true;
    } else if (len > s->value.ext.capacity) {
      if constexpr (Opts.preserve) {
        const auto newcap = std::max(len, s->length * 2);
        auto* new_buf = vrt_alloc_charbuf(newcap);
        std::copy_n(s->value.ext.data, s->length, new_buf);
        vrt_unifree(s->value.ext.data);
        s->value.ext.capacity = newcap;
      } else {
        vrt_unifree(s->value.ext.data);
        s->value.ext.data = vrt_alloc_charbuf(len);
        s->value.ext.capacity = len;
      }
    }
  }
  s->length = len;
}

vrt_octetstring_t make_dummy_octetstring(octetstring_size_t len) {
  vrt_octetstring_t r;
  r.is_bound = true;
  r.is_ext = false;
  octetstring_update_length(&r, len);
  return r;
}
}  // namespace

const vrt_typeinfo_t octetstring_typeinfo{
    .name = "octetstring",
    .kind = vrt_typekind_e::kScalar,
    .bytes = sizeof(vrt_octetstring_t),

    .members = nullptr,

    .construct = rt::helpers::VoidErased<vrt_octetstring_ctor>,
    .destruct = rt::helpers::VoidErased<vrt_octetstring_dtor>,
};

void vrt_octetstring_ctor(vrt_octetstring_t* p) {
  p->is_bound = false;
  p->is_ext = false;  // that's why not "is_sso" - if we just zero memory, is_sso would be false
  p->length = 0;
}
void vrt_octetstring_dtor(vrt_octetstring_t* p) {
  if (p->is_bound && p->is_ext) {
    vrt_unifree(p->value.ext.data);
  }
}

octet_t* vrt_octetstring_get_buf(vrt_octetstring_t* s) {
  return s->is_ext ? s->value.ext.data : s->value.intl;
}
const octet_t* vrt_octetstring_get_cbuf(const vrt_octetstring_t* s) {
  return vrt_octetstring_get_buf(const_cast<vrt_octetstring_t*>(s));
}

void vrt_octetstring_assign(vrt_octetstring_t* dst, const octet_t* src, octetstring_size_t len) {
  octetstring_update_length(dst, len);
  std::copy_n(src, len, vrt_octetstring_get_buf(dst));
}

void copy_octetstring(vrt_octetstring_t* dst, const vrt_octetstring_t* src) {
  AssertIsBound(src);
  dst->is_bound = true;
  vrt_octetstring_assign(dst, vrt_octetstring_get_cbuf(src), src->length);
}

extern "C" {
void vrt_octetstring_init(vrt_octetstring_t* dst, const octet_t* src, octetstring_size_t len) {
  dst->is_ext = false;
  dst->is_bound = true;
  vrt_octetstring_assign(dst, src, len);
}
}

void vrt_octetstring_concat(vrt_octetstring_t* dst, const vrt_octetstring_t* a, const vrt_octetstring_t* b) {
  AssertIsBound(a);
  AssertIsBound(b);

  // TODO: optimize "a := a & ..." and "a := ... & a" (append/prepend)

  const auto total_len = a->length + b->length;

  auto tmp = make_dummy_octetstring(total_len);

  octetstring_update_length(&tmp, total_len);

  octet_t* dst_buf = vrt_octetstring_get_buf(&tmp);
  dst_buf = std::copy_n(vrt_octetstring_get_cbuf(a), a->length, dst_buf);
  dst_buf = std::copy_n(vrt_octetstring_get_cbuf(b), b->length, dst_buf);

  //
  *dst = tmp;
}

octet_t vrt_octetstring_at(const vrt_octetstring_t* s, octetstring_size_t i) {
  AssertIsBound(s);

  if (i >= s->length) {
    rt::Panic(
        "Index overflow when accessing an octetstring element: the index is {}, but the string has "
        "only {} octets",
        i, s->length);
    return {};
  }

  return vrt_octetstring_get_cbuf(s)[i];
}

void vrt_octetstring_singular(vrt_octetstring_t* dst, const vrt_octetstring_t* s, octetstring_size_t i) {
  octet_t v = vrt_octetstring_at(s, i);

  dst->is_bound = true;
  dst->is_ext = false;

  dst->value.intl[0] = v;
  dst->length = 1;
}

namespace {
void vrt_octetstring_shift_base(vrt_octetstring_t* dst, const vrt_octetstring_t* s, std::int64_t n, auto do_shift,
                                auto do_shift_inv) {
  AssertIsBound(s);
  const auto len = s->length;

  if (len == 0 || n == 0) {
    vrt_octetstring_t tmp;
    copy_octetstring(&tmp, s);
    *dst = tmp;
    return;
  }

  auto tmp = make_dummy_octetstring(len);

  const auto* srcbuf = vrt_octetstring_get_cbuf(s);
  auto* buf = vrt_octetstring_get_buf(&tmp);

  n = std::min<std::int64_t>(n, len);
  if (n < 0) {
    do_shift_inv(srcbuf, buf, len, -n);
  } else {
    do_shift(srcbuf, buf, len, n % len);
  }

  *dst = tmp;
}
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
  vrt_octetstring_shift_base(dst, s, n,  //
                             vrt_octetstring_shift_left_impl, vrt_octetstring_shift_right_impl);
}

void vrt_octetstring_shift_right(vrt_octetstring_t* dst, const vrt_octetstring_t* s, std::int64_t n) {
  vrt_octetstring_shift_base(dst, s, n,  //
                             vrt_octetstring_shift_right_impl, vrt_octetstring_shift_left_impl);
}

namespace {
void vrt_octetstring_rotate_base(vrt_octetstring_t* dst, const vrt_octetstring_t* s, std::int64_t n, auto do_rotate,
                                 auto do_rotate_inv) {
  AssertIsBound(s);
  const auto len = s->length;

  if (len == 0 || n == 0) {
    vrt_octetstring_t tmp;
    copy_octetstring(&tmp, s);
    *dst = tmp;
    return;
  }

  auto tmp = make_dummy_octetstring(len);

  const auto* srcbuf = vrt_octetstring_get_cbuf(s);
  auto* buf = vrt_octetstring_get_buf(&tmp);

  n %= len;
  if (n < 0) {
    do_rotate_inv(srcbuf, buf, len, -n);
  } else {
    do_rotate(srcbuf, buf, len, n % len);
  }

  *dst = tmp;
}
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
  vrt_octetstring_rotate_base(dst, s, n,  //
                              vrt_octetstring_rotate_left_impl, vrt_octetstring_rotate_right_impl);
}

void vrt_octetstring_rotate_right(vrt_octetstring_t* dst, const vrt_octetstring_t* s, std::int64_t n) {
  vrt_octetstring_rotate_base(dst, s, n,  //
                              vrt_octetstring_rotate_right_impl, vrt_octetstring_rotate_left_impl);
}

void vrt_octetstring_set(vrt_octetstring_t* s, octetstring_size_t i, octet_t v) {
  AssertIsBound(s);

  if (i > s->length) {
    rt::Panic(
        "Index overflow when accessing an octetstring element: the index is {}, but the string has "
        "only {} octets",
        i, s->length);
    return;
  }

  if (s->length == i) {
    octetstring_update_length(s, s->length + 1);
  }

  vrt_octetstring_get_buf(s)[i] = v;
}

bool vrt_octetstring_eq(const vrt_octetstring_t* lhs, const vrt_octetstring_t* rhs) {
  AssertIsBound(lhs);
  AssertIsBound(rhs);
  return (lhs->length == rhs->length) &&
         std::equal(vrt_octetstring_get_cbuf(lhs), vrt_octetstring_get_cbuf(lhs) + lhs->length,
                    vrt_octetstring_get_cbuf(rhs));
}

//

void vrt_octetstring_not4b(vrt_octetstring_t* dst, const vrt_octetstring_t* s) {
  auto tmp = make_dummy_octetstring(s->length);

  const auto* srcbuf = vrt_octetstring_get_cbuf(s);
  auto* buf = vrt_octetstring_get_buf(&tmp);

  for (octetstring_size_t i = 0; i < s->length; ++i) {
    buf[i] = ~srcbuf[i];
  }

  *dst = tmp;
}

namespace {
template <typename F>
  requires requires(F f) {
    { f(std::declval<octet_t>(), std::declval<octet_t>()) } -> std::same_as<octet_t>;
  }
void vrt_octetstring_binop(vrt_octetstring_t* dst, const vrt_octetstring_t* lhs, const vrt_octetstring_t* rhs,
                           std::string_view op_name, F apply) {
  AssertIsBound(lhs);
  AssertIsBound(rhs);

  rt::Assert(lhs->length == rhs->length, "{} operands must have the same length", op_name);
  const auto len = lhs->length;

  auto tmp = make_dummy_octetstring(len);

  auto* buf = vrt_octetstring_get_buf(&tmp);
  const auto* lbuf = vrt_octetstring_get_cbuf(lhs);
  const auto* rbuf = vrt_octetstring_get_cbuf(rhs);

  for (octetstring_size_t i = 0; i < len; ++i) {
    buf[i] = apply(lbuf[i], rbuf[i]);
  }

  *dst = tmp;
}
}  // namespace

void vrt_octetstring_and4b(vrt_octetstring_t* dst, const vrt_octetstring_t* lhs, const vrt_octetstring_t* rhs) {
  vrt_octetstring_binop(dst, lhs, rhs, "and4b", std::bit_and<octet_t>{});
}
void vrt_octetstring_or4b(vrt_octetstring_t* dst, const vrt_octetstring_t* lhs, const vrt_octetstring_t* rhs) {
  vrt_octetstring_binop(dst, lhs, rhs, "or4b", std::bit_or<octet_t>{});
}
void vrt_octetstring_xor4b(vrt_octetstring_t* dst, const vrt_octetstring_t* lhs, const vrt_octetstring_t* rhs) {
  vrt_octetstring_binop(dst, lhs, rhs, "xor4b", std::bit_xor<octet_t>{});
}

// NOLINTEND(readability-identifier-naming)
