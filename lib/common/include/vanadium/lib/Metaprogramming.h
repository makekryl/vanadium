#pragma once

#include <type_traits>

namespace mp {

template <class... Ts>
struct Overloaded : Ts... {
  using Ts::operator()...;
};

template <typename... Ts>
struct Typelist {
  template <typename Fn>
  static void Apply(Fn f) {
    (f.template operator()<Ts>(), ...);
  }
};

template <template <typename...> class Target, typename TList>
struct Apply;

template <template <typename...> class Target, typename... Ts>
struct Apply<Target, Typelist<Ts...>> {
  using type = Target<Ts...>;
};

template <typename F, typename R, typename... Args>
concept Invocable = std::is_invocable_r_v<R, F, Args...>;

template <typename F, typename... Args>
concept Consumer = Invocable<F, void, Args...>;

}  // namespace mp
