#include "vanadium/runtime/rt_charstring.h"

#include <algorithm>
#include <format>
#include <string_view>
#include <utility>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

// NOLINTBEGIN(readability-identifier-naming)

namespace {
constexpr std::uint32_t kSmallCapacity = sizeof(std::declval<vrt_charstring_t>().value.intl);  // is not zero-terminated

inline void AssertIsBound(const vrt_charstring_t* s) {
  if (!s->is_bound) [[unlikely]] {
    rt::Panic("accessing an unbound charstring value");
  }
}

char* vrt_alloc_charbuf(std::uint32_t len) {
  return static_cast<char*>(vrt_alloc(len, 1));
}

struct LengthUpdateOpts {
  bool realloc{true};
};

template <LengthUpdateOpts Opts>
void charstring_update_length(vrt_charstring_t* s, std::uint32_t len) {
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

const vrt_typeinfo_t charstring_typeinfo{
    .name = "charstring",
    .kind = vrt_typekind_e::kScalar,
    .bytes = sizeof(vrt_charstring_t),

    .members = nullptr,

    .construct = rt::helpers::VoidErased<vrt_charstring_ctor>,
    .destruct = rt::helpers::VoidErased<vrt_charstring_dtor>,
};

void vrt_charstring_ctor(vrt_charstring_t* p) {
  p->is_bound = false;
  p->is_ext = false;  // that's why not "is_sso" - if we just zero memory, is_sso would be false
}
void vrt_charstring_dtor(vrt_charstring_t* p) {
  if (p->is_bound && p->is_ext) {
    vrt_unifree(p->value.ext.data);
  }
}

char* vrt_charstring_get_buf(vrt_charstring_t* s) {
  return s->is_ext ? s->value.ext.data : s->value.intl;
}
const char* vrt_charstring_get_cbuf(const vrt_charstring_t* s) {
  return vrt_charstring_get_buf(const_cast<vrt_charstring_t*>(s));
}

void vrt_charstring_assign(vrt_charstring_t* dst, const char* src, std::uint32_t len) {
  charstring_update_length<{.realloc = false}>(dst, len);
  std::copy_n(src, len, vrt_charstring_get_buf(dst));
}

void copy_charstring(vrt_charstring_t* dst, const vrt_charstring_t* src) {
  AssertIsBound(src);
  vrt_charstring_assign(dst, vrt_charstring_get_cbuf(src), src->length);
}

extern "C" {
void vrt_charstring_init(vrt_charstring_t* dst, const char* src, std::uint32_t len) {
  dst->is_ext = false;
  dst->is_bound = true;
  vrt_charstring_assign(dst, src, len);
}
}

void vrt_charstring_concat(vrt_charstring_t* dst, const vrt_charstring_t* a, const vrt_charstring_t* b) {
  AssertIsBound(a);
  AssertIsBound(b);

  // TODO: optimize "a := a & ..." and "a := ... & a" (append/prepend)

  const auto total_len = a->length + b->length;

  vrt_charstring_t tmp;
  tmp.is_bound = true;
  tmp.is_ext = false;

  charstring_update_length<{.realloc = false}>(&tmp, total_len);

  char* dst_buf = vrt_charstring_get_buf(&tmp);
  dst_buf = std::copy_n(vrt_charstring_get_cbuf(a), a->length, dst_buf);
  dst_buf = std::copy_n(vrt_charstring_get_cbuf(b), b->length, dst_buf);

  //
  *dst = tmp;
}

char vrt_charstring_at(const vrt_charstring_t* s, std::uint32_t i) {
  AssertIsBound(s);

  if (i >= s->length) {
    rt::Panic(
        "Index overflow when accessing a charstring element: the index is {}, but the string has "
        "only {} characters",
        i, s->length);
    return {};
  }

  return vrt_charstring_get_cbuf(s)[i];
}

void vrt_charstring_singular(vrt_charstring_t* dst, const vrt_charstring_t* s, std::uint32_t i) {
  char v = vrt_charstring_at(s, i);

  dst->is_bound = true;
  dst->is_ext = false;

  dst->value.intl[0] = v;
  dst->length = 1;
}

void vrt_charstring_set(vrt_charstring_t* s, std::uint32_t i, char v) {
  AssertIsBound(s);

  if (i > s->length) {
    rt::Panic(
        "Index overflow when accessing a charstring element: the index is {}, but the string has "
        "only {} characters",
        i, s->length);
    return;
  }

  if (s->length == i) {
    charstring_update_length<{.realloc = false}>(s, s->length + 1);
  }

  vrt_charstring_get_buf(s)[i] = v;
}

bool vrt_charstring_eq(const vrt_charstring_t* lhs, const vrt_charstring_t* rhs) {
  AssertIsBound(lhs);
  AssertIsBound(rhs);
  return std::string_view{vrt_charstring_get_cbuf(lhs), lhs->length} ==
         std::string_view{vrt_charstring_get_cbuf(rhs), rhs->length};
}

// NOLINTEND(readability-identifier-naming)
