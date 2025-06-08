#pragma once

#include <ranges>
#include <ryml.hpp>
#include <string_view>
#include <type_traits>

#include "c4/format.hpp"
#include "c4/substr.hpp"
#include "c4/substr_fwd.hpp"
#include "c4/yml/node.hpp"
#include "c4/yml/node_type.hpp"

namespace lsp {

using LiteralStub = std::nullptr_t;

namespace detail {

struct StructWrapper {
  StructWrapper(ryml::NodeRef n) : n_(std::move(n)) {}

  void init() {
    n_ |= ryml::MAP;
  }

  [[nodiscard]] ryml::NodeRef jsonNode() const {
    return n_;
  }

 protected:
  ryml::NodeRef n_;
};

template <typename... Ts>
struct UnionStructWrapper : StructWrapper {
  using StructWrapper::StructWrapper;
};

template <typename EnumName>
void SerializeEnum(EnumName, ryml::NodeRef);
template <typename EnumName>
EnumName DeserializeEnum(ryml::ConstNodeRef);

struct NodeRange {
  mutable ryml::NodeRef node;

  struct Iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = ryml::NodeRef;
    using difference_type = std::ptrdiff_t;
    using reference = ryml::NodeRef;

    ryml::NodeRef::iterator it;

    Iterator() noexcept : it(nullptr, 0) {};
    Iterator(ryml::NodeRef::iterator it_) noexcept : it(std::move(it_)) {};

    reference operator*() const {
      return *it;
    }
    Iterator& operator++() {
      ++it;
      return *this;
    }
    void operator++(int) noexcept {
      ++it;
    }
    bool operator==(const Iterator& other) const {
      return it == other.it;
    }
  };

  [[nodiscard]] Iterator begin() {
    return {node.begin()};
  }
  [[nodiscard]] Iterator end() {
    return {node.end()};
  }
};

template <typename T>
concept IsPrimitive = std::is_same_v<T, std::string_view>   //
                      || std::is_same_v<T, bool>            //
                      || std::is_same_v<T, double>          //
                      || std::is_same_v<T, std::int32_t>    //
                      || std::is_same_v<T, std::uint32_t>;  //

template <IsPrimitive T>
T NodeAsPrimitive(ryml::ConstNodeRef n) {
  T t;
  n >> t;
  return t;
}
template <>
inline std::string_view NodeAsPrimitive(ryml::ConstNodeRef n) {
  const auto val = n.val();
  return {val.data(), val.size()};
}

template <IsPrimitive T>
void SetPrimitive(ryml::NodeRef n, T val) {
  n << val;
}
template <>
inline void SetPrimitive(ryml::NodeRef n, bool val) {
  n << ryml::fmt::boolalpha(val);
}
template <>
inline void SetPrimitive(ryml::NodeRef n, std::string_view val) {
  n.set_val(ryml::csubstr(val.data(), val.length()));
}

}  // namespace detail

template <typename T>
struct List : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  T emplace()
    requires(!detail::IsPrimitive<T>)
  {
    return T(n_.append_child());
  }

  void emplace(T item)
    requires std::is_enum_v<T>
  {
    detail::SerializeEnum(item, n_.append_child());
  }

  void emplace(T item)
    requires detail::IsPrimitive<T>
  {
    detail::SetPrimitive(n_.append_child(), item);
  }

  auto range()
    requires detail::IsPrimitive<T>
  {
    return detail::NodeRange{n_} | std::views::transform([](ryml::NodeRef c) {
             return NodeAsPrimitive<T>(c);
           });
  }

  auto range()
    requires std::is_enum_v<T>
  {
    return detail::NodeRange{n_} | std::views::transform([](ryml::NodeRef c) {
             return detail::DeserializeEnum<T>(c);
           });
  }

  auto range() {
    return detail::NodeRange{n_} | std::views::transform([](ryml::NodeRef c) {
             return T(std::move(c));
           });
  }

  void init() {
    n_ |= ryml::SEQ;
  }
};

template <typename K, typename V>
struct Dict : detail::StructWrapper {
  using StructWrapper::StructWrapper;

  void init() {
    n_ |= ryml::MAP;
  }
};

struct LSPAny : detail::StructWrapper {
  using StructWrapper::StructWrapper;
  void init() {}
};
using LSPArray = List<LSPAny>;
using LSPObject = Dict<std::string_view, LSPArray>;

}  // namespace lsp
