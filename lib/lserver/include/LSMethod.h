#pragma once

#include <vanadium/lib/jsonrpc/Common.h>

namespace vanadium::lserver {

namespace rpc {

template <typename T>
using ExpectedResult = std::expected<T, lib::jsonrpc::Error>;

template <glz::string_literal Name, typename Params, typename Result>
struct Method {
  static constexpr auto kMethodName = Name;
  using TParams = Params;
  using TResult = Result;
};

struct Empty {};

}  // namespace rpc

}  // namespace vanadium::lserver
