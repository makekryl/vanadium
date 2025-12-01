#pragma once

#include <oneapi/tbb/concurrent_unordered_map.h>
#include <oneapi/tbb/task_arena.h>
#include <oneapi/tbb/task_group.h>
#include <vanadium/lib/jsonrpc/Common.h>

#include <condition_variable>
#include <cstddef>
#include <glaze/json.hpp>

#include "LSChannel.h"
#include "LSMessageToken.h"
#include "LSTransport.h"

namespace vanadium::lserver {

template <typename TContextPayload>
class Connection;

template <typename Payload>
class ConnectionContext {
  using TConnection = Connection<Payload>;

 public:
  ConnectionContext(TConnection* server);

  PooledMessageToken AcquireToken() {
    return pool_.Acquire();
  }
  void Send(PooledMessageToken&&);
  [[nodiscard]] std::size_t GetConnectionConcurrency() const;

  template <glz::string_literal Method, typename Result, typename Params>
  std::expected<Result, lib::jsonrpc::Error> Request(Params&& params);

  template <glz::string_literal Method, typename Params>
  void Notify(Params&& params);

  Payload& operator&() {
    return data_;
  }
  const Payload& operator&() const {
    return data_;
  }

  Payload* operator->() {
    return &data_;
  }
  const Payload* operator->() const {
    return &data_;
  }

 private:
  TConnection* const connection_;
  TokenPool pool_;
  Payload data_;
};

template <typename TContextPayload>
class Connection {
 public:
  using Context = ConnectionContext<TContextPayload>;
  using HandlerFn = std::function<void(Context&, PooledMessageToken&&)>;

  Connection(HandlerFn handler, Transport& transport, std::size_t concurrency, std::size_t backlog)
      : handler_(handler),
        channel_(transport, concurrency * 2),
        backlog_(backlog),
        task_arena_(kServiceWorkerThreads + concurrency),
        pending_outbound_requests_(concurrency, nullptr),
        ctx_(this) {}

  Connection(const Connection&) = delete;
  Connection(Connection&&) = delete;
  Connection& operator=(const Connection&) = delete;
  Connection& operator=(Connection&&) = delete;

  void Listen() {
    is_running_ = true;

    task_arena_.execute([&] {
      wg_.run([&] {
        while (is_running_.load()) {
          channel_.Read();
        }
      });
      wg_.run([&] {
        while (is_running_.load()) {
          channel_.Write();
        }
      });

      const auto concurrency = GetConcurrency();
      for (std::size_t i = 0; i < concurrency; ++i) {
        wg_.run([&, j = i] {
          while (is_running_.load()) {
            PooledMessageToken token;
            inbound_requests_queue_.pop(token);
            handler_(ctx_, std::move(token));
          }
        });
      }
    });

    // TODO: potential deadlock - if all workers are waiting for outbound request response,
    //       and the backlog is full, connection will deadlock
    while (is_running_.load()) {
      auto token = channel_.Poll();
      if (!pending_outbound_requests_.empty()) [[unlikely]] {
        auto inbound_request = MaybeRouteOutboundRequestResponse(std::move(token));
        if (!inbound_request) {
          continue;
        }
        token = std::move(*inbound_request);
      }
      inbound_requests_queue_.emplace(std::move(token));
    }
  }

  void Stop() {
    is_running_ = false;
    wg_.cancel();
  }

  [[nodiscard]] std::size_t GetConcurrency() const noexcept {
    return task_arena_.max_concurrency() - kServiceWorkerThreads;
  }
  [[nodiscard]] std::size_t GetBacklog() const noexcept {
    return backlog_;
  }

  Context& GetContext() {
    return ctx_;
  }

  //

  void Send(PooledMessageToken&& token) {
    channel_.Enqueue(std::move(token));
  }

  template <glz::string_literal Method, typename Result, typename Params>
  std::expected<Result, lib::jsonrpc::Error> Request(Params&& params) {
    const auto id = outbound_id_++;
    lib::jsonrpc::Request<Params> req{
        .id = id,
        .method = Method,
        .params = std::forward<Params>(params),
    };
    PooledMessageToken res_token;

    {
      auto req_token = ctx_.AcquireToken();
      if (auto err = glz::write_json(req, req_token->buf)) [[unlikely]] {
        return glz::unexpected(lib::jsonrpc::Error{
            .code = lib::jsonrpc::ErrorCode::kInvalidParams,
            .data = glz::format_error(err, req_token->buf),
        });
      }

      res_token = RawRequest(id, std::move(req_token));
    }

    lib::jsonrpc::Response<Result> res;
    if (auto err = glz::read_json<>(res, res_token->buf)) [[unlikely]] {
      return std::unexpected(lib::jsonrpc::Error{
          .code = lib::jsonrpc::ErrorCode::kParseError,
          .data = glz::format_error(err, res_token->buf),
      });
    }

    if (!res.result.has_value()) [[unlikely]] {
      return std::unexpected(res.error.value());
    }

    return res.result.value();
  }

  template <glz::string_literal Method, typename Params>
  void Notify(Params&& params) {
    lib::jsonrpc::Notification<Params> req{
        .method = Method,
        .params = std::forward<Params>(params),
    };

    auto token = ctx_.AcquireToken();
    if (auto err = glz::write_json(req, token->buf)) {
      std::abort();  // TODO
    }
    Send(std::move(token));
  }

 private:
  // TODO: according to the jsonrpc spec, id can also be a string
  using rpc_id_t = std::uint32_t;

  PooledMessageToken RawRequest(rpc_id_t id, PooledMessageToken&& token) {
    WaitToken wait_token{.awaited_id = id};
    pending_outbound_requests_[tbb::this_task_arena::current_thread_index()] = &wait_token;
    Send(std::move(token));

    {
      std::unique_lock lock(pending_outbound_requests_mutex_);
      pending_outbound_requests_cv_.wait(lock, [&] {
        return wait_token.satisfied.load(std::memory_order_acquire);
      });
    }

    return std::move(*wait_token.response);
  }

  std::optional<PooledMessageToken> MaybeRouteOutboundRequestResponse(PooledMessageToken&& token) {
    const auto result = glz::get_as_json<rpc_id_t, "/result">(token->buf);
    if (!result.has_value()) {
      return token;
    }

    const auto id = glz::get_as_json<rpc_id_t, "/id">(token->buf);
    if (!id.has_value()) {
      // invalid response, todo: check
      std::abort();
      return std::nullopt;
    }

    for (auto& pending : pending_outbound_requests_) {
      if (!pending || pending->awaited_id != *id) {
        continue;
      }
      pending->response = std::move(token);
      pending->satisfied.store(true, std::memory_order_release);
      pending = nullptr;
      {
        std::lock_guard lock(pending_outbound_requests_mutex_);
        pending_outbound_requests_cv_.notify_all();
      }
      break;
    }

    return std::nullopt;
  }

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

  Context ctx_;
};

template <typename Payload>
ConnectionContext<Payload>::ConnectionContext(TConnection* server)
    : connection_(server), pool_(server->GetConcurrency() * server->GetBacklog()) {}

template <typename Payload>
void ConnectionContext<Payload>::Send(PooledMessageToken&& token) {
  connection_->Send(std::move(token));
}

template <typename Payload>
std::size_t ConnectionContext<Payload>::GetConnectionConcurrency() const {
  return connection_->GetConcurrency();
}

template <typename Payload>
template <glz::string_literal Method, typename Result, typename Params>
std::expected<Result, lib::jsonrpc::Error> ConnectionContext<Payload>::Request(Params&& params) {
  return connection_->template Request<Method, Result, Params>(std::forward<Params>(params));
}

template <typename Payload>
template <glz::string_literal Method, typename Params>
void ConnectionContext<Payload>::Notify(Params&& params) {
  return connection_->template Notify<Method, Params>(std::forward<Params>(params));
}

}  // namespace vanadium::lserver
