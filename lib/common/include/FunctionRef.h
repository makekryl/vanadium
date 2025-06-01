#pragma once

#include <cstddef>

namespace vanadium::lib {

template <typename>
class FunctionRef;

template <typename TReturn, typename... TParams>
class FunctionRef<TReturn(TParams...)> final {
 private:
  using TStub = TReturn (*)(void* instance, TParams...);

 public:
  constexpr FunctionRef(std::nullptr_t) {};

  template <typename Func>
  FunctionRef(const Func& func) : FunctionRef((void*)(&func), lambda_stub<Func>) {}

  constexpr operator bool() const noexcept {
    return stub_ != nullptr;
  }

  TReturn operator()(TParams... arg) const {
    return (*stub_)(instance_, arg...);
  }

  template <class T, TReturn (T::*TMethod)(TParams...)>
  constexpr static FunctionRef create(T* instance) {
    return FunctionRef(instance, method_stub<T, TMethod>);
  }

  template <class T, TReturn (T::*TMethod)(TParams...) const>
  constexpr static FunctionRef create(T const* instance) {
    return FunctionRef(const_cast<T*>(instance), const_method_stub<T, TMethod>);
  }

  template <TReturn (*TMethod)(TParams...)>
  constexpr static FunctionRef create() {
    return FunctionRef(nullptr, function_stub<TMethod>);
  }

  template <typename Func>
  constexpr static FunctionRef create(const Func& func) {
    return FunctionRef(func);
  }

 private:
  constexpr FunctionRef(void* instance, TStub stub) {
    instance_ = instance;
    stub_ = stub;
  }

  template <TReturn (*TMethod)(TParams...)>
  constexpr static TReturn function_stub(void*, TParams... params) {
    return (TMethod)(params...);
  }

  template <typename Func>
  constexpr static TReturn lambda_stub(void* instance, TParams... arg) {
    Func* p = static_cast<Func*>(instance);
    return (p->operator())(arg...);
  }

  template <class T, TReturn (T::*TMethod)(TParams...)>
  constexpr static TReturn method_stub(void* instance, TParams... params) {
    auto* p = static_cast<T*>(instance);
    return (p->*TMethod)(params...);
  }

  template <class T, TReturn (T::*TMethod)(TParams...) const>
  constexpr static TReturn const_method_stub(void* instance, TParams... params) {
    auto* const p = static_cast<T*>(instance);
    return (p->*TMethod)(params...);
  }

  void* instance_ = nullptr;
  TStub stub_ = nullptr;
};

template <typename... TParams>
using Consumer = FunctionRef<void(TParams...)>;

template <typename... TParams>
using Predicate = FunctionRef<bool(TParams...)>;

};  // namespace vanadium::lib
