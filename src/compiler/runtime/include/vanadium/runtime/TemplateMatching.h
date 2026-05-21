#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ranges>
#include <type_traits>
#include <utility>

#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/rt_template.h"

namespace vanadium::rt::tpl {

template <typename T>
struct ValueList;

template <typename T>
struct Implication;

template <typename T>
concept RtTemplate = requires(T t) {
  { auto(t.tsel) } -> std::same_as<vrt_template_sel_e>;
  { auto(t.list) } -> std::same_as<ValueList<T>>;
  { auto(t.implication) } -> std::same_as<Implication<T>*>;
  { auto(t.dynmatch) } -> std::same_as<vrt_dynmatcher_t>;
};
template <RtTemplate T>
using RtTemplateType = decltype(std::declval<T>().val);

struct GenericTemplateType;

template <typename T>
struct ValueList {
  T* data;
  vrt_valuelist_size_t length;
  std::uint32_t esize;  // actually u64, but it's nice to save up padded 8 bytes

  [[nodiscard]] auto Range() const
    requires(std::is_same_v<T, GenericTemplateType>)
  {
    return std::views::iota(vrt_valuelist_size_t{0}, length) | std::views::transform([&](auto i) -> T& {
             return *reinterpret_cast<T*>(reinterpret_cast<std::byte*>(data) + (i * esize));
           });
  }
  [[nodiscard]] auto Range() const
    requires(!std::is_same_v<T, GenericTemplateType>)
  {
    return std::ranges::subrange(data, data + length);
  }

  template <auto Matcher, typename V>
  bool MatchAny(const V* v) const {
    return std::ranges::any_of(Range(), [&v](const auto& e) {
      return Matcher(v, &e);
    });
  }

  template <auto Matcher, typename V>
  bool MatchAll(const V* v) const {
    return std::ranges::all_of(Range(), [&v](const auto& e) {
      return Matcher(v, &e);
    });
  }

  void Release(auto Destruct) {
    for (auto& p : Range()) {
      Destruct(&p);
    }
    vrt_unifree(data);
  }
};

template <typename T>
struct Implication {
  T precondition;
  T implied;

  template <auto Matcher, typename V>
  bool Match(const V* v) const {
    return !Matcher(v, &precondition) || Matcher(v, &implied);
  }

  void Release(auto Destruct) {
    Destruct(&precondition);
    Destruct(&implied);
  }
};

//

template <auto Matcher, RtTemplate T>
bool Match(const RtTemplateType<T>* v, const T* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kOmitValue:
      return false;

    case vrt_template_sel_e::kAnyValue:
    case vrt_template_sel_e::kAnyOrOmit:
      return true;

    case vrt_template_sel_e::kValueList:
      return t->list.template MatchAny<Matcher>(v);
    case vrt_template_sel_e::kComplementedList:
      return !t->list.template MatchAny<Matcher>(v);

    case vrt_template_sel_e::kConjunctionList:
      return t->list.template MatchAll<Matcher>(v);

    case vrt_template_sel_e::kImplication:
      return t->implication->template Match<Matcher>(v);

    case vrt_template_sel_e::kDynamic:
      return t->dynmatch.match(t->dynmatch.ctx, v);

    default:
      return Matcher(v, t);
  }
}

void Construct(RtTemplate auto* t) {
  t->tsel = vrt_template_sel_e::kUninitialized;
}

void Destruct(auto Destructor, RtTemplate auto* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kUninitialized:
    case vrt_template_sel_e::kOmitValue:
    case vrt_template_sel_e::kAnyValue:
    case vrt_template_sel_e::kAnyOrOmit:
      break;
    case vrt_template_sel_e::kValueList:
    case vrt_template_sel_e::kComplementedList:
    case vrt_template_sel_e::kConjunctionList:
      t->list.Release(Destructor);
      break;
    case vrt_template_sel_e::kImplication:
      t->implication->Release(Destructor);
      vrt_unifree(t->implication);
      break;
    case vrt_template_sel_e::kDynamic:
      vrt_unifree(t->dynmatch.ctx);
      break;
    default:
      assert(false);
  }
}

template <auto Destructor>
void Destruct(RtTemplate auto* t) {
  Destruct(Destructor, t);
}

struct GenericTemplateType {
  vrt_template_sel_e tsel;

  union {
    void* val;  // ptr for proper alignment
    vanadium::rt::tpl::ValueList<GenericTemplateType> list;
    vanadium::rt::tpl::Implication<GenericTemplateType>* implication;
    vrt_dynmatcher_t dynmatch;
  };

  void* GetPayload() {
    return static_cast<void*>(&val);
  }
};

inline GenericTemplateType* AsGenericTemplateType(void* p) {
  return static_cast<rt::tpl::GenericTemplateType*>(p);
}

inline bool IsTemplateType(const vrt_typeinfo_t* td) {
  return td->tpl_construct_value != nullptr;
}

}  // namespace vanadium::rt::tpl
