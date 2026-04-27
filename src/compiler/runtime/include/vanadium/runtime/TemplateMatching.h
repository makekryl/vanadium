#pragma once

#include <algorithm>
#include <cassert>
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

struct GenericTemplateTypeLayout {
  vrt_template_sel_e tsel;
  void* dp;  // ptr for alignment

  void* GetPayload() {
    return static_cast<void*>(&dp);
  }
};

template <typename T>
struct ValueList {
  T* data;
  vrt_valuelist_size_t length;

  template <auto Matcher, typename V>
  bool MatchAny(const V* v) const {
    return std::any_of(data, data + length, [&v](const auto& e) {
      return Matcher(v, &e);
    });
  }

  template <auto Matcher, typename V>
  bool MatchAll(const V* v) const {
    return std::all_of(data, data + length, [&v](const auto& e) {
      return Matcher(v, &e);
    });
  }

  template <auto Destruct>
  void Release() {
    for (vrt_valuelist_size_t i = 0; i < length; ++i) {
      Destruct(&data[i]);
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

  template <auto Destruct>
  void Release() {
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

template <auto Destructor>
void Destruct(RtTemplate auto* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kUninitialized:
    case vrt_template_sel_e::kOmitValue:
    case vrt_template_sel_e::kAnyValue:
    case vrt_template_sel_e::kAnyOrOmit:
      break;
    case vrt_template_sel_e::kValueList:
    case vrt_template_sel_e::kComplementedList:
    case vrt_template_sel_e::kConjunctionList:
      t->list.template Release<Destructor>();
      break;
    case vrt_template_sel_e::kImplication:
      t->implication->template Release<Destructor>();
      vrt_unifree(t->implication);
      break;
    case vrt_template_sel_e::kDynamic:
      vrt_unifree(t->dynmatch.ctx);
      break;
    default:
      assert(false);
  }
}

}  // namespace vanadium::rt::tpl
