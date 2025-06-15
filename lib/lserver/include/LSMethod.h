#pragma once

#include <glaze/ext/jsonrpc.hpp>
#include <glaze/util/expected.hpp>

namespace vanadium::lserver {

namespace rpc {

template <typename T>
using ExpectedResult = glz::expected<T, glz::rpc::error>;

template <glz::string_literal Name, typename Params, typename Result>
struct Method {
  using RpcMethod = glz::rpc::method<Name, Params, Result>;

  static constexpr auto kMethodName = Name;
  using TParams = Params;
  using TResult = Result;
};

struct Empty {};

}  // namespace rpc

}  // namespace vanadium::lserver
