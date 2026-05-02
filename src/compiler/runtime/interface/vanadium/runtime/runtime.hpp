#pragma once

#include <cstdint>
#include <format>
#include <string_view>
#include <utility>

#include "vanadium/runtime/rt_charstring.h"
#include "vanadium/runtime/rt_octetstring.h"
#include "vanadium/runtime/runtime.h"

namespace vanadium::rt {

// TODO: real, configurable & low-latency logger
template <typename... Args>
void Log(std::format_string<Args...> fstr, Args&&... args) {
  vrt_log_write(std::format(fstr, std::forward<Args>(args)...).c_str());
}

template <typename... Args>
void Panic(std::format_string<Args...> fstr, Args&&... args) {
  return vrt_panic(std::format(fstr, std::forward<Args>(args)...).c_str());
}

template <typename... Args>
void Assert(bool cond, std::format_string<Args...> fstr, Args&&... args) {
  if (!cond) [[unlikely]] {
    Panic(fstr, std::forward<Args>(args)...);
  }
}

template <typename T>
  requires requires(T t) { requires std::same_as<decltype(t.is_bound), bool>; }
class ValueWrapper {
 public:
  ValueWrapper(const ValueWrapper&) = delete;
  ValueWrapper& operator=(ValueWrapper) = delete;

  [[nodiscard]] bool IsBound() const {
    return value_.is_bound;
  }
  void SetBound() {
    value_.is_bound = true;
  }

  T& unwrap() {
    return value_;
  }
  [[nodiscard]] const T& unwrap() const {
    return value_;
  }

 protected:
  ValueWrapper() = default;
  explicit ValueWrapper(T&& value) : value_(std::move(value)) {}

  T value_;
};

class Charstring : public ValueWrapper<vrt_charstring_t> {
 public:
  Charstring() {
    charstring_ctor(&value_);
  }
  Charstring(vrt_charstring_t&& instance) : ValueWrapper(std::move(instance)) {}

  Charstring(const Charstring& other) : ValueWrapper() {
    copy_charstring(&value_, &other.value_);
  }

  Charstring(Charstring&& other) noexcept {
    value_ = std::move(other.value_);
    other.value_.is_bound = false;
  }

  ~Charstring() {
    charstring_dtor(&value_);
  }

  [[nodiscard]] operator std::string_view() const {
    return {vrt_charstring_get_cbuf(&value_), value_.length};
  }
  [[nodiscard]] operator std::span<const char>() const {
    return {vrt_charstring_get_cbuf(&value_), value_.length};
  }
  [[nodiscard]] operator std::span<char>() {
    return {vrt_charstring_get_buf(&value_), value_.length};
  }

  Charstring& operator=(Charstring other) {
    Charstring tmp(std::move(other));
    std::swap(value_, tmp.value_);
    return *this;
  }

  [[nodiscard]] bool operator==(const Charstring& other) const {
    return vrt_charstring_eq(&value_, &other.value_);
  }
  [[nodiscard]] bool operator!=(const Charstring& other) const {
    return !(*this == other);
  }

  Charstring operator+(const Charstring& other) const {
    vrt_charstring_t tmp;
    vrt_charstring_concat(&tmp, &value_, &other.value_);
    return tmp;
  }
  Charstring& operator+=(const Charstring& other) {
    // TODO: use optimized routines
    vrt_charstring_t tmp;
    vrt_charstring_concat(&tmp, &value_, &other.value_);
    *this = std::move(tmp);
    return *this;
  }

  Charstring operator<<(std::int64_t n) const {
    vrt_charstring_t tmp;
    vrt_charstring_rotate_left(&tmp, &value_, n);
    return tmp;
  }
  Charstring& operator<<=(std::int64_t n) {
    // TODO: use optimized routines
    vrt_charstring_t tmp;
    vrt_charstring_rotate_left(&tmp, &value_, n);
    *this = std::move(tmp);
    return *this;
  }
  Charstring operator>>(std::int64_t n) const {
    vrt_charstring_t tmp;
    vrt_charstring_rotate_right(&tmp, &value_, n);
    return tmp;
  }
  Charstring& operator>>=(std::int64_t n) {
    // TODO: use optimized routines
    vrt_charstring_t tmp;
    vrt_charstring_rotate_right(&tmp, &value_, n);
    *this = std::move(tmp);
    return *this;
  }

  [[nodiscard]] std::uint32_t length() const {
    return value_.length;
  }

  class ElementProxy {
   public:
    operator char() {
      return std::as_const(*s_)[i_];
    }
    ElementProxy& operator=(char c) {
      vrt_charstring_set(&s_->unwrap(), i_, c);
      return *this;
    }

   private:
    ElementProxy(Charstring* s, std::uint32_t i) : s_(s), i_(i) {}

    Charstring* s_;
    std::uint32_t i_;

    friend class Charstring;
  };

  [[nodiscard]] char operator[](std::uint32_t i) const {
    return vrt_charstring_at(&value_, i);
  }
  [[nodiscard]] ElementProxy operator[](std::uint32_t i) {
    return {this, i};
  }
};

class Octetstring : public ValueWrapper<vrt_octetstring_t> {
 public:
  Octetstring() {
    octetstring_ctor(&value_);
  }
  Octetstring(vrt_octetstring_t&& instance) : ValueWrapper(std::move(instance)) {}

  Octetstring(const Octetstring& other) : ValueWrapper() {
    copy_octetstring(&value_, &other.value_);
  }

  Octetstring(Octetstring&& other) noexcept {
    value_ = std::move(other.value_);
    other.value_.is_bound = false;
  }

  ~Octetstring() {
    octetstring_dtor(&value_);
  }

  [[nodiscard]] operator std::span<const octet_t>() const {
    return {vrt_octetstring_get_cbuf(&value_), value_.length};
  }
  [[nodiscard]] operator std::span<octet_t>() {
    return {vrt_octetstring_get_buf(&value_), value_.length};
  }

  Octetstring& operator=(Octetstring other) {
    Octetstring tmp(std::move(other));
    std::swap(value_, tmp.value_);
    return *this;
  }

  [[nodiscard]] bool operator==(const Octetstring& other) const {
    return vrt_octetstring_eq(&value_, &other.value_);
  }
  [[nodiscard]] bool operator!=(const Octetstring& other) const {
    return !(*this == other);
  }

  Octetstring operator+(const Octetstring& other) const {
    vrt_octetstring_t tmp;
    vrt_octetstring_concat(&tmp, &value_, &other.value_);
    return tmp;
  }
  Octetstring& operator+=(const Octetstring& other) {
    // TODO: use optimized routines
    vrt_octetstring_t tmp;
    vrt_octetstring_concat(&tmp, &value_, &other.value_);
    *this = std::move(tmp);
    return *this;
  }

  Octetstring operator<<(std::int64_t n) const {
    vrt_octetstring_t tmp;
    vrt_octetstring_rotate_left(&tmp, &value_, n);
    return tmp;
  }
  Octetstring& operator<<=(std::int64_t n) {
    // TODO: use optimized routines
    vrt_octetstring_t tmp;
    vrt_octetstring_rotate_left(&tmp, &value_, n);
    *this = std::move(tmp);
    return *this;
  }
  Octetstring operator>>(std::int64_t n) const {
    vrt_octetstring_t tmp;
    vrt_octetstring_rotate_right(&tmp, &value_, n);
    return tmp;
  }
  Octetstring& operator>>=(std::int64_t n) {
    // TODO: use optimized routines
    vrt_octetstring_t tmp;
    vrt_octetstring_rotate_right(&tmp, &value_, n);
    *this = std::move(tmp);
    return *this;
  }

  [[nodiscard]] std::uint32_t length() const {
    return value_.length;
  }

  class ElementProxy {
   public:
    operator char() {
      return std::as_const(*s_)[i_];
    }
    ElementProxy& operator=(char c) {
      vrt_octetstring_set(&s_->unwrap(), i_, c);
      return *this;
    }

   private:
    ElementProxy(Octetstring* s, std::uint32_t i) : s_(s), i_(i) {}

    Octetstring* s_;
    std::uint32_t i_;

    friend class Octetstring;
  };

  [[nodiscard]] char operator[](std::uint32_t i) const {
    return vrt_octetstring_at(&value_, i);
  }
  [[nodiscard]] ElementProxy operator[](std::uint32_t i) {
    return {this, i};
  }
};

}  // namespace vanadium::rt

// NOLINTNEXTLINE(misc-unused-alias-decls)
namespace rt = vanadium::rt;
