#pragma once

#include <condition_variable>
#include <cstddef>
#include <expected>
#include <type_traits>

#include <glaze/json.hpp>
#include <oneapi/tbb/concurrent_unordered_map.h>
#include <oneapi/tbb/spin_rw_mutex.h>
#include <oneapi/tbb/task_arena.h>
#include <oneapi/tbb/task_group.h>

#include <vanadium/lib/jsonrpc/Common.h>

#include "vanadium/lib/lserver/Channel.h"
#include "vanadium/lib/lserver/MessageToken.h"
#include "vanadium/lib/lserver/Transport.h"

namespace vanadium::lserver {

// template <typename T>
struct NoAwaitResponse {};

class Connection {
 public:
  using HandlerFn = std::function<void(Connection&, PooledMessageToken&&)>;
  using rpc_id_t = std::uint32_t;

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
    const auto assign_wait_token = [&](WaitToken* tokenptr) {
      std::unique_lock l(channel_read_mutex_);
      pending_outbound_requests_[tbb::this_task_arena::current_thread_index()] = tokenptr;
    };
    const auto free_wait_token = [&] {
      assign_wait_token(nullptr);
    };

    //

    const auto id = outbound_id_++;
    const auto send_req = [&] {
      return SendRequest<Method>(id, std::forward<Params>(params));
    };

    if constexpr (std::is_same_v<Result, NoAwaitResponse>) {
      if (auto err = send_req(); err) [[unlikely]] {
        return std::unexpected{*err};
      }
      return {};
    }

    WaitToken wait_token{.awaited_id = id};
    assign_wait_token(&wait_token);

    if (auto err = send_req(); err) [[unlikely]] {
      free_wait_token();
      return std::unexpected{*err};
    }

    {
      std::unique_lock lock(pending_outbound_requests_mutex_);
      pending_outbound_requests_cv_.wait(lock, [&] {
        return wait_token.satisfied.load(std::memory_order_acquire);
      });
    }
    free_wait_token();

    auto res_token = std::move(*wait_token.response);

    lib::jsonrpc::Response<Result> res;
    if (glz::error_ctx err = glz::read_json<>(res, res_token->buf)) [[unlikely]] {
      // TODO: it does not seem correct to return jsonrpc::Error from the point of the client
      //       as it would think it was sent by the server and not produced internally
      return std::unexpected{lib::jsonrpc::Error{
          .code = lib::jsonrpc::ErrorCode::kParseError,
          .data = glz::format_error(err, res_token->buf),
      }};
    }

    if (res.error.has_value()) [[unlikely]] {
      return std::unexpected{*res.error};
    }

    if constexpr (std::is_same_v<Result, std::nullptr_t>) {
      return nullptr;
    } else {
      if (!res.result.has_value()) [[unlikely]] {
        return std::unexpected{lib::jsonrpc::Error{
            .code = lib::jsonrpc::ErrorCode::kParseError,
            .data = "result is missing from response",
        }};
      }
      return *res.result;
    }
  }

 private:
  template <glz::string_literal Method, typename Params>
  std::optional<lib::jsonrpc::Error> SendRequest(rpc_id_t id, Params&& params) {
    const lib::jsonrpc::Request<Params> req{
        .id = id,
        .method = Method,
        .params = std::forward<Params>(params),
    };

    auto req_token = AcquireToken();
    if (auto err = glz::write_json(req, req_token->buf)) [[unlikely]] {
      return lib::jsonrpc::Error{
          .code = lib::jsonrpc::ErrorCode::kInvalidParams,
          .data = glz::format_error(err, req_token->buf),
      };
    }
    Send(std::move(req_token));

    return std::nullopt;
  }

  [[nodiscard]] bool AwaitsResponse() const noexcept;
  [[nodiscard]] std::optional<PooledMessageToken> MaybeRouteOutboundRequestResponse(PooledMessageToken&& token);

  struct WaitToken {
    rpc_id_t awaited_id;
    std::optional<PooledMessageToken> response{std::nullopt};
    std::atomic<bool> satisfied{false};
  };

  HandlerFn handler_;

  Channel channel_;

  std::size_t backlog_;
  tbb::task_arena task_arena_;
  tbb::task_group wg_;
  static constexpr decltype(task_arena_.max_concurrency()) kServiceWorkerThreads = 3;  // Read+Write+Poll

  std::atomic<bool> is_running_;

  // It suspends incoming message routing during initiating a Server->Client request
  tbb::speculative_spin_rw_mutex channel_read_mutex_;

  tbb::concurrent_bounded_queue<PooledMessageToken> inbound_requests_queue_;

  std::atomic<rpc_id_t> outbound_id_{1};
  std::vector<WaitToken*> pending_outbound_requests_;  // <-- should be protected by exclusive channel_read_mutex_
  std::mutex pending_outbound_requests_mutex_;
  std::condition_variable pending_outbound_requests_cv_;

  TokenPool pool_;
};

}  // namespace vanadium::lserver
