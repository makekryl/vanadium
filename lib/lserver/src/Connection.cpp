#include "vanadium/lib/lserver/Connection.h"

#include <condition_variable>
#include <cstddef>

#include "vanadium/lib/lserver/Channel.h"
#include "vanadium/lib/lserver/MessageToken.h"
#include "vanadium/lib/lserver/Transport.h"

namespace vanadium::lserver {

Connection::Connection(HandlerFn handler, Transport& transport, std::size_t concurrency, std::size_t backlog)
    : handler_(std::move(handler)),
      channel_(transport, concurrency * 2),
      backlog_(backlog),
      task_arena_(kServiceWorkerThreads + concurrency),
      pending_outbound_requests_(concurrency, nullptr),
      pool_(GetConcurrency() * GetBacklog()) {}

void Connection::Listen() {
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
      wg_.run([&] {
        while (is_running_.load()) {
          PooledMessageToken token;
          inbound_requests_queue_.pop(token);
          handler_(*this, std::move(token));
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

void Connection::Stop() {
  is_running_ = false;
  wg_.cancel();
}

PooledMessageToken Connection::AwaitRpcResponse(rpc_id_t id) {
  WaitToken wait_token{.awaited_id = id};
  pending_outbound_requests_[tbb::this_task_arena::current_thread_index()] = &wait_token;

  {
    std::unique_lock lock(pending_outbound_requests_mutex_);
    pending_outbound_requests_cv_.wait(lock, [&] {
      return wait_token.satisfied.load(std::memory_order_acquire);
    });
  }

  return std::move(*wait_token.response);
}

std::optional<PooledMessageToken> Connection::MaybeRouteOutboundRequestResponse(PooledMessageToken&& token) {
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

}  // namespace vanadium::lserver
