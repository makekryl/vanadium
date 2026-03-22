#pragma once

namespace vanadium::rt::helpers {

template <typename>
struct ctor_dtor_fn_traits;

template <typename T>
struct ctor_dtor_fn_traits<void (*)(T*)> {
  using arg_type = T;
};

template <auto F>
void VoidErased(void* p) {
  using T = typename ctor_dtor_fn_traits<decltype(F)>::arg_type;
  F(static_cast<T*>(p));
}

}  // namespace vanadium::rt::helpers
