#pragma once

#include <glaze/json.hpp>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>

#include "vanadium/lib/jsonrpc/Common.h"

// Based on <glaze/ext/jsonrpc.hpp>

namespace vanadium::lib::jsonrpc {

template <typename TContext>
class Server {
 public:
  Server() = default;

  template <auto Fn>
  struct RequestHandlerTraits;

  template <typename TParams, typename TResult, TResult (*Fn)(TContext&, const TParams&)>
  struct RequestHandlerTraits<Fn> {
    using Params = TParams;
    using Result = TResult;
  };

  template <auto HandlerFn>
    requires requires {
      typename RequestHandlerTraits<HandlerFn>::Params;
      typename RequestHandlerTraits<HandlerFn>::Result;
    }
  void Bind(std::string_view method) {
    assert(!handlers_.contains(method) && "Method is already bound");
    handlers_[method] = &Invoke<HandlerFn>;
  }

  void Unbind(std::string_view method) {
    assert(handlers_.contains(method) && "Method is not bound");
    handlers_.erase(method);
  }

  [[nodiscard]] bool IsBound(std::string_view method) const noexcept {
    return handlers_.contains(method);
  }

  std::optional<std::string_view> Call(TContext& ctx, std::string& res_buffer, std::string_view json_request) {
    const auto write_json = [&]<typename TRet>(TRet&& response) -> std::string_view {
      if (const glz::error_ctx err = glz::write_json(std::forward<TRet>(response), res_buffer); err) {
        res_buffer = glz::format_error(err);
      }
      return res_buffer;
    };

    if (auto parse_err{glz::validate_json(json_request)}) {
      return write_json(GenericResponse{.error = Error{
                                            .code = ErrorCode::kParseError,
                                            .data = glz::format_error(parse_err, json_request),
                                        }});
    }

    const glz::expected<GenericRequest, glz::error_ctx> request{glz::read_json<GenericRequest>(json_request)};

    if (!request.has_value()) {
      GenericResponse res{.error = Error::invalid(request.error(), json_request)};
      auto id{glz::get_as_json<id_t, "/id">(json_request)};
      if (id.has_value()) {
        res.id = std::move(*id);
      }
      return write_json(res);
    }

    if (request->version != kJsonRpcVersion) {
      return write_json(GenericResponse{.id = std::move(request->id), .error = Error::version(request->version)});
    }

    auto it = handlers_.find(request->method);
    if (it == handlers_.end()) {
      return write_json(GenericResponse{.id = std::move(request->id), .error = Error::method(request->method)});
    }

    const auto& handler = it->second;

    const auto result = handler(ctx, request->id, json_request, res_buffer);
    if (std::holds_alternative<std::string_view>(result)) {
      return std::get<std::string_view>(result);
    }
    if (std::holds_alternative<GenericResponse>(result)) {
      return write_json(std::get<GenericResponse>(result));
    }
    if (std::holds_alternative<std::monostate>(result)) {
      return std::nullopt;
    }
    std::unreachable();
  }

 private:
  template <auto HandlerFn>
  static std::variant<std::monostate, GenericResponse, std::string_view> Invoke(TContext& ctx, id_t req_id,
                                                                                std::string_view req_json,
                                                                                std::string& res_buffer) {
    using Params = typename RequestHandlerTraits<HandlerFn>::Params;
    using Result = typename RequestHandlerTraits<HandlerFn>::Result;

    const glz::expected<Request<Params>, glz::error_ctx>& params_req{glz::read_json<Request<Params>>(req_json)};
    if (!params_req.has_value()) {
      return GenericResponse{.id = std::move(req_id), .error = Error::invalid(params_req.error(), req_json)};
    }

    const auto invoke_handler = [&] -> auto {
      return HandlerFn(ctx, params_req->params);
    };

    if constexpr (std::is_same_v<Result, void>) {
      // Notification
      invoke_handler();
      return std::monostate{};
    } else {
      const std::expected<Result, Error> result = invoke_handler();
      if (!result.has_value()) {
        return GenericResponse{.id = std::move(req_id), .error = std::move(result.error())};
      }

      const Response<Result> response{.id = std::move(req_id), .result = std::move(*result)};
      if (const glz::error_ctx& err = glz::write_json(response, res_buffer); err) [[unlikely]] {
        return GenericResponse{.id = std::move(req_id),
                               .error = Error{
                                   .code = ErrorCode::kInternal,
                                   .data = format_error(err),
                                   .message = "Failed to serialize response",
                               }};
      }

      return res_buffer;
    }
  }

  using InternalHandlerFn = std::variant<std::monostate, GenericResponse, std::string_view> (*)(
      TContext&,         // ctx
      id_t,              // req_id
      std::string_view,  // req_json
      std::string&       // res_buffer
  );
  std::unordered_map<std::string_view, InternalHandlerFn> handlers_;
};
};  // namespace vanadium::lib::jsonrpc
