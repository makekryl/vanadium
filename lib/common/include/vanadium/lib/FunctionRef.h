#pragma once

#include <cstddef>
#include <utility>

namespace vanadium::lib {

template <typename>
class FunctionRef;

template <typename TReturn, typename... TParams>
class FunctionRef<TReturn(TParams...)> final {
 private:
  using TSignature = TReturn (*)(void* instance, TParams...);

 public:
  constexpr FunctionRef(std::nullptr_t) {};

  template <typename Func>
  FunctionRef(const Func& func) : FunctionRef((void*)(&func), invoke_lambda<Func>) {}

  constexpr operator bool() const noexcept {
    return vptr_ != nullptr;
  }

  TReturn operator()(TParams... arg) const {
    return (*vptr_)(instance_, arg...);
  }

  template <class T, TReturn (T::*TMethod)(TParams...)>
  constexpr static FunctionRef create(T* instance) {
    return FunctionRef(instance, invoke_method<T, TMethod>);
  }

  template <class T, TReturn (T::*TMethod)(TParams...) const>
  constexpr static FunctionRef create(T const* instance) {
    return FunctionRef(const_cast<T*>(instance), invoke_const_method<T, TMethod>);
  }

  template <TReturn (*TMethod)(TParams...)>
  constexpr static FunctionRef create() {
    return FunctionRef(nullptr, invoke_fn<TMethod>);
  }

  template <typename Func>
  constexpr static FunctionRef create(const Func& func) {
    return FunctionRef(func);
  }

 private:
  constexpr FunctionRef(void* instance, TSignature vptr) : instance_(instance), vptr_(vptr) {}

  template <TReturn (*TMethod)(TParams...)>
  constexpr static TReturn invoke_fn(void*, TParams... params) {
    return (TMethod)(std::forward<TParams>(params)...);
  }

  template <typename Func>
  constexpr static TReturn invoke_lambda(void* instance, TParams... params) {
    Func* p = static_cast<Func*>(instance);
    return (p->operator())(std::forward<TParams>(params)...);
  }

  template <class T, TReturn (T::*TMethod)(TParams...)>
  constexpr static TReturn invoke_method(void* instance, TParams... params) {
    auto* p = static_cast<T*>(instance);
    return (p->*TMethod)(std::forward<TParams>(params)...);
  }

  template <class T, TReturn (T::*TMethod)(TParams...) const>
  constexpr static TReturn invoke_const_method(void* instance, TParams... params) {
    auto* const p = static_cast<T*>(instance);
    return (p->*TMethod)(std::forward<TParams>(params)...);
  }

  void* instance_ = nullptr;
  TSignature vptr_ = nullptr;
};

template <typename... TParams>
using Consumer = FunctionRef<void(TParams...)>;

template <typename... TParams>
using Predicate = FunctionRef<bool(TParams...)>;

};  // namespace vanadium::lib
