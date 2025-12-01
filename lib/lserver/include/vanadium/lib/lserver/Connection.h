#pragma once

#include <oneapi/tbb/concurrent_unordered_map.h>
#include <oneapi/tbb/task_arena.h>
#include <oneapi/tbb/task_group.h>
#include <vanadium/lib/jsonrpc/Common.h>

#include <condition_variable>
#include <cstddef>
#include <expected>
#include <glaze/json.hpp>
#include <type_traits>

#include "vanadium/lib/lserver/Channel.h"
#include "vanadium/lib/lserver/MessageToken.h"
#include "vanadium/lib/lserver/Transport.h"

namespace vanadium::lserver {

class Connection {
 public:
  using HandlerFn = std::function<void(Connection&, PooledMessageToken&&)>;

  Connection(HandlerFn handler, Transport& transport, std::size_t concurrency, std::size_t backlog);

  Connection(const Connection&) = delete;
  Connection(Connection&&) = delete;
  Connection& operator=(const Connection&) = delete;
  Connection& operator=(Connection&&) = delete;

  void Listen();
  void Stop();

  [[nodiscard]] std::size_t GetConcurrency() const noexcept {
    return task_arena_.max_concurrency() - kServiceWorkerThreads;
  }
  [[nodiscard]] std::size_t GetBacklog() const noexcept {
    return backlog_;
  }

  //

  PooledMessageToken AcquireToken() {
    return pool_.Acquire();
  }

  //

  void Send(PooledMessageToken&& token) {
    channel_.Enqueue(std::move(token));
  }

  template <glz::string_literal Method, typename Params>
  void Notify(Params&& params) {
    lib::jsonrpc::Notification<Params> req{
        .method = Method,
        .params = std::forward<Params>(params),
    };

    auto token = AcquireToken();
    if (auto err = glz::write_json(req, token->buf)) {
      std::abort();  // TODO
    }
    Send(std::move(token));
  }

  template <glz::string_literal Method, typename Result, typename Params>
  std::expected<Result, lib::jsonrpc::Error> Request(Params&& params) {
    const auto id = SendRequest<Method>(std::move(params));
    if (!id.has_value()) [[unlikely]] {
      return std::unexpected{id.error()};
    }

    if constexpr (std::is_same_v<Result, lib::jsonrpc::Empty>) {
      return Result{};
    }
    auto res_token = AwaitRpcResponse(*id);

    lib::jsonrpc::Response<Result> res;
    if (glz::error_ctx err = glz::read_json<>(res, res_token->buf)) [[unlikely]] {
      return std::unexpected{lib::jsonrpc::Error{
          .code = lib::jsonrpc::ErrorCode::kParseError,
          .data = glz::format_error(err, res_token->buf),
      }};
    }

    if (!res.result.has_value()) [[unlikely]] {
      return std::unexpected{res.error.value()};
    }

    return res.result.value();
  }

 private:
  using rpc_id_t = std::uint32_t;

  template <glz::string_literal Method, typename Params>
  std::expected<rpc_id_t, lib::jsonrpc::Error> SendRequest(Params&& params) {
    const auto id = outbound_id_++;
    const lib::jsonrpc::Request<Params> req{
        .id = id,
        .method = Method,
        .params = std::forward<Params>(params),
    };

    auto req_token = AcquireToken();
    if (auto err = glz::write_json(req, req_token->buf)) [[unlikely]] {
      return std::unexpected{lib::jsonrpc::Error{
          .code = lib::jsonrpc::ErrorCode::kInvalidParams,
          .data = glz::format_error(err, req_token->buf),
      }};
    }
    Send(std::move(req_token));

    return id;
  }

  PooledMessageToken AwaitRpcResponse(rpc_id_t id);
  std::optional<PooledMessageToken> MaybeRouteOutboundRequestResponse(PooledMessageToken&& token);

  struct WaitToken {
    rpc_id_t awaited_id;
    std::optional<PooledMessageToken> response{std::nullopt};
    std::atomic<bool> satisfied;
  };

  HandlerFn handler_;

  Channel channel_;

  std::size_t backlog_;
  tbb::task_arena task_arena_;
  tbb::task_group wg_;
  static constexpr decltype(task_arena_.max_concurrency()) kServiceWorkerThreads = 3;  // Read+Write+Poll

  std::atomic<bool> is_running_;

  tbb::concurrent_bounded_queue<PooledMessageToken> inbound_requests_queue_;

  std::atomic<rpc_id_t> outbound_id_{1};
  std::vector<WaitToken*> pending_outbound_requests_;
  std::mutex pending_outbound_requests_mutex_;
  std::condition_variable pending_outbound_requests_cv_;

  TokenPool pool_;
};

}  // namespace vanadium::lserver
