#pragma once

namespace vanadium::rt::helpers {

template <typename T>
struct erase_arg;

template <typename T>
struct erase_arg<T*> {
  using type = void*;
};

template <typename T>
struct erase_arg<const T*> {
  using type = const void*;
};

template <typename T>
using erase_arg_t = typename erase_arg<T>::type;

template <typename>
struct fn_traits;

template <typename R, typename... Args>
struct fn_traits<R (*)(Args...)> {
  using erased_fn = R (*)(erase_arg_t<Args>...);
};

template <auto F>
struct void_erased;

template <typename R, typename... Args, R (*F)(Args...)>
struct void_erased<F> {
  static R call(erase_arg_t<Args>... args) {
    return F(static_cast<Args>(args)...);
  }

  static constexpr auto value = &call;
};

template <auto F>
inline constexpr auto void_erased_v = void_erased<F>::value;

}  // namespace vanadium::rt::helpers
