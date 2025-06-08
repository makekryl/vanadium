#pragma once

#include <ryml.hpp>
#include <string_view>
#include <type_traits>

#include "LSProtocolBase.h"
#include "c4/yml/node_type.hpp"

namespace jsonrpc {

using id_t = std::uint32_t;
using lsp::detail::NodeAsPrimitive;

struct JsonRpcObject {
  explicit JsonRpcObject(ryml::NodeRef n) : n_(std::move(n)) {}

  [[nodiscard]] std::string_view jsonrpc() const {
    return NodeAsPrimitive<std::string_view>(n_["jsonrpc"]);
  }

  //

  [[nodiscard]] ryml::NodeRef jsonNode() {
    return n_;
  }

  template <typename T = JsonRpcObject>
    requires std::is_base_of_v<JsonRpcObject, T>
  static T Create(ryml::NodeRef n) {
    n |= ryml::MAP;

    T o(n);
    o.n_.append_child() << ryml::key("jsonrpc") << "2.0";
    return o;
  }

 protected:
  ryml::NodeRef n_;
};

struct Notification : JsonRpcObject {
  explicit Notification(ryml::NodeRef n) : JsonRpcObject(n) {}

  [[nodiscard]] std::string_view method() const {
    return NodeAsPrimitive<std::string_view>(n_["method"]);
  }

  [[nodiscard]] bool has_params() const {
    return n_.has_child("params");
  }
  [[nodiscard]] ryml::NodeRef params() {
    return n_["params"];
  }

  //

  template <typename T = Notification>
    requires std::is_base_of_v<Notification, T>
  static T Create(ryml::NodeRef n, std::string_view method) {
    auto o = JsonRpcObject::Create<T>(n);
    o.n_.append_child() << ryml::key("method") << ryml::csubstr(method.data(), method.length());
    return o;
  }

  template <typename T = Notification>
    requires std::is_base_of_v<Notification, T>
  static T CreateWithParams(ryml::NodeRef n, std::string_view method) {
    auto o = Notification::Create(n, method);
    o.n_.append_child() << ryml::key("params");
    return o;
  }
};

struct Request final : Notification {
  explicit Request(ryml::NodeRef n) : Notification(std::move(n)) {}
  Request(Notification notification) : Notification(std::move(notification)) {}

  [[nodiscard]] id_t id() const {
    return NodeAsPrimitive<id_t>(n_["id"]);
  }

  //

  static Request Create(ryml::NodeRef n, std::string_view method, id_t id) {
    auto o = Notification::Create<Request>(n, method);
    o.n_.append_child() << ryml::key("id") << id;
    return o;
  }
  static Request CreateWithParams(ryml::NodeRef n, std::string_view method, id_t id) {
    auto o = Request::Create(n, method, id);
    o.n_.append_child() << ryml::key("params");
    return o;
  }
};

struct Response final : JsonRpcObject {
  explicit Response(ryml::NodeRef n) : JsonRpcObject(std::move(n)) {}

  [[nodiscard]] bool has_result() const {
    return n_.has_child("result");
  }
  [[nodiscard]] ryml::NodeRef result() {
    return n_["result"];
  }

  [[nodiscard]] bool has_error() const {
    return n_.has_child("error");
  }
  [[nodiscard]] ryml::NodeRef error() {
    return n_["error"];
  }

  [[nodiscard]] id_t id() const {
    return NodeAsPrimitive<id_t>(n_["id"]);
  }

  //

  static Response Success(ryml::NodeRef n, id_t id) {
    auto o = Response::Create(n, id);
    o.n_.append_child() << ryml::key("result");
    return o;
  }

  static Response Failure(ryml::NodeRef n, id_t id) {
    auto o = Response::Create(n, id);
    o.n_.append_child() << ryml::key("error");
    return o;
  }

 protected:
  static Response Create(ryml::NodeRef n, id_t id) {
    auto o = JsonRpcObject::Create<Response>(n);
    o.n_.append_child() << ryml::key("id") << id;
    return o;
  }
};

}  // namespace jsonrpc
