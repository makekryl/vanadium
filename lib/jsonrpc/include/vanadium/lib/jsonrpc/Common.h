#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

#include <glaze/json.hpp>

// Based on <glaze/ext/jsonrpc.hpp>

namespace vanadium::lib::jsonrpc {

constexpr std::string_view kJsonRpcVersion{"2.0"};

using id_t = std::variant<glz::generic::null_t, std::string_view, std::int64_t>;

//

enum class ErrorCode : std::int16_t {
  kNoError = 0,
  kServerErrorLower = -32000,
  kServerErrorUpper = -32099,
  kInvalidRequest = -32600,
  kMethodNotFound = -32601,
  kInvalidParams = -32602,
  kInternal = -32603,
  kParseError = -32700,
};

constexpr std::string_view StringifyError(ErrorCode err) noexcept {
  switch (err) {
    case ErrorCode::kNoError:
      return "No error";
    case ErrorCode::kParseError:
      return "Parse error";
    case ErrorCode::kServerErrorLower:
    case ErrorCode::kServerErrorUpper:
      return "Server error";
    case ErrorCode::kInvalidRequest:
      return "Invalid request";
    case ErrorCode::kMethodNotFound:
      return "Method not found";
    case ErrorCode::kInvalidParams:
      return "Invalid params";
    case ErrorCode::kInternal:
      return "Internal error";
  }
  return "Unknown";
}

struct Error final {
  ErrorCode code{ErrorCode::kNoError};
  std::optional<std::string> data;
  std::string message{StringifyError(code)};

  static Error invalid(const glz::error_ctx& pe, auto& buffer) {
    std::string format_err{format_error(pe, buffer)};
    return {.code = ErrorCode::kInvalidRequest,
            .data = format_err.empty() ? std::nullopt : std::optional{format_err},
            .message = std::string(StringifyError(ErrorCode::kInvalidRequest))};
  }
  static Error version(std::string_view presumed_version) {
    return {.code = ErrorCode::kInvalidRequest,
            .data = "Invalid version: " + std::string(presumed_version) + " only supported version is " +
                    std::string(kJsonRpcVersion),
            .message = std::string(StringifyError(ErrorCode::kInvalidRequest))};
  }
  static Error method(std::string_view presumed_method) {
    return {.code = ErrorCode::kMethodNotFound,
            .data = "Method: '" + std::string(presumed_method) + "' not found",
            .message = std::string(StringifyError(ErrorCode::kMethodNotFound))};
  }

  operator bool() const noexcept {
    return code != ErrorCode::kNoError;
  }

  bool operator==(ErrorCode err) const noexcept {
    return code == err;
  }

  // NOLINTBEGIN(readability-identifier-naming)
  struct glaze {
    static constexpr auto value = glz::object(&Error::code, &Error::message, &Error::data);
  };
  // NOLINTEND(readability-identifier-naming)
};

//

template <class Params>
struct Request {
  id_t id;
  std::string_view method;
  Params params;
  std::string_view version{kJsonRpcVersion};

  // NOLINTBEGIN(readability-identifier-naming)
  struct glaze {
    static constexpr auto value = glz::object("jsonrpc", &Request::version,  //
                                              &Request::method,              //
                                              &Request::params,              //
                                              &Request::id);
  };
  // NOLINTEND(readability-identifier-naming)
};

template <class Params>
Request(id_t&&, std::string_view, Params&&) -> Request<std::decay_t<Params>>;

using GenericRequest = Request<glz::raw_json_view>;

//

template <class Params>
struct Notification {
  std::string_view method;
  Params params;
  std::string_view version{kJsonRpcVersion};

  // NOLINTBEGIN(readability-identifier-naming)
  struct glaze {
    static constexpr auto value = glz::object("jsonrpc", &Notification::version,  //
                                              &Notification::method,              //
                                              &Notification::params);
  };
  // NOLINTEND(readability-identifier-naming)
};

template <class Params>
Notification(std::string_view, Params&&) -> Notification<std::decay_t<Params>>;

using GenericNotification = Notification<glz::raw_json_view>;

//

template <class Result>
struct Response {
  id_t id;
  std::optional<Result> result{};
  std::optional<Error> error;
  std::string version{kJsonRpcVersion};

  // NOLINTBEGIN(readability-identifier-naming)
  struct glaze {
    static constexpr auto value{glz::object("jsonrpc", &Response::version,  //
                                            &Response::result,              //
                                            &Response::error,               //
                                            &Response::id)};
  };
  // NOLINTEND(readability-identifier-naming)
};

using GenericResponse = Response<glz::raw_json_view>;

//

struct Empty {};

};  // namespace vanadium::lib::jsonrpc
