#include "vanadium/runtime/rt_octetstring.h"

#include <algorithm>
#include <format>
#include <utility>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

// NOLINTBEGIN(readability-identifier-naming)

namespace {
constexpr std::uint32_t kSmallCapacity =
    sizeof(std::declval<vrt_octetstring_t>().value.intl);  // is not zero-terminated

inline void AssertIsBound(const vrt_octetstring_t* s) {
  if (!s->is_bound) [[unlikely]] {
    rt::Panic("accessing an unbound octetstring value");
  }
}

octet_t* vrt_alloc_charbuf(std::uint32_t len) {
  return static_cast<octet_t*>(vrt_alloc(len, 1));
}

struct LengthUpdateOpts {
  bool realloc{true};
};

template <LengthUpdateOpts Opts>
void octetstring_update_length(vrt_octetstring_t* s, std::uint32_t len) {
  if (len <= kSmallCapacity) {
    if (s->is_ext) {
      auto* ext_buf = s->value.ext.data;
      if constexpr (Opts.realloc) {
        std::copy_n(ext_buf, s->length, s->value.intl);
      }
      vrt_unifree(ext_buf);
      s->is_ext = false;
    }
  } else {
    if (!s->is_ext) {
      auto* ext_buf = vrt_alloc_charbuf(len);
      if constexpr (Opts.realloc) {
        std::copy_n(s->value.intl, s->length, ext_buf);
      }
      s->value.ext.data = ext_buf;
      s->value.ext.capacity = len;
      s->is_ext = true;
    } else if (len > s->value.ext.capacity) {
      if constexpr (Opts.realloc) {
        auto* new_buf = vrt_alloc_charbuf(len);
        std::copy_n(s->value.ext.data, s->length, new_buf);
        vrt_unifree(s->value.ext.data);
      } else {
        vrt_unifree(s->value.ext.data);
        s->value.ext.data = vrt_alloc_charbuf(len);
        s->value.ext.capacity = len;
      }
    }
  }
  s->length = len;
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

void vrt_octetstring_assign(vrt_octetstring_t* dst, const octet_t* src, std::uint32_t len) {
  octetstring_update_length<{.realloc = false}>(dst, len);
  std::copy_n(src, len, vrt_octetstring_get_buf(dst));
}

void copy_octetstring(vrt_octetstring_t* dst, const vrt_octetstring_t* src) {
  AssertIsBound(src);
  dst->is_bound = true;
  vrt_octetstring_assign(dst, vrt_octetstring_get_cbuf(src), src->length);
}

extern "C" {
void vrt_octetstring_init(vrt_octetstring_t* dst, const octet_t* src, std::uint32_t len) {
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

  vrt_octetstring_t tmp;
  tmp.is_bound = true;
  tmp.is_ext = false;

  octetstring_update_length<{.realloc = false}>(&tmp, total_len);

  octet_t* dst_buf = vrt_octetstring_get_buf(&tmp);
  dst_buf = std::copy_n(vrt_octetstring_get_cbuf(a), a->length, dst_buf);
  dst_buf = std::copy_n(vrt_octetstring_get_cbuf(b), b->length, dst_buf);

  //
  *dst = tmp;
}

octet_t vrt_octetstring_at(const vrt_octetstring_t* s, std::uint32_t i) {
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

void vrt_octetstring_singular(vrt_octetstring_t* dst, const vrt_octetstring_t* s, std::uint32_t i) {
  octet_t v = vrt_octetstring_at(s, i);

  dst->is_bound = true;
  dst->is_ext = false;

  dst->value.intl[0] = v;
  dst->length = 1;
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

  vrt_octetstring_t tmp;
  tmp.is_bound = true;
  tmp.is_ext = s->is_ext;
  octetstring_update_length<{.realloc = false}>(&tmp, len);

  const auto* srcbuf = vrt_octetstring_get_cbuf(s);
  auto* buf = vrt_octetstring_get_buf(&tmp);

  if (n < 0) {
    do_rotate_inv(srcbuf, buf, len, -n);
    return;
  }

  do_rotate(srcbuf, buf, len, n % len);

  *dst = tmp;
}
void vrt_octetstring_rotate_left_impl(const octet_t* srcbuf, octet_t* buf, std::uint32_t len, std::int64_t n) {
  std::copy_n(srcbuf + n, len - n, buf);
  std::copy_n(srcbuf, n, buf + len - n);
}
void vrt_octetstring_rotate_right_impl(const octet_t* srcbuf, octet_t* buf, std::uint32_t len, std::int64_t n) {
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

void vrt_octetstring_set(vrt_octetstring_t* s, std::uint32_t i, octet_t v) {
  AssertIsBound(s);

  if (i > s->length) {
    rt::Panic(
        "Index overflow when accessing an octetstring element: the index is {}, but the string has "
        "only {} octets",
        i, s->length);
    return;
  }

  if (s->length == i) {
    octetstring_update_length<{.realloc = false}>(s, s->length + 1);
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

// NOLINTEND(readability-identifier-naming)
