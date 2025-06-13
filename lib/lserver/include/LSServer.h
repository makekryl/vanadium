#pragma once

#include <oneapi/tbb/task_arena.h>
#include <oneapi/tbb/task_group.h>

#include <c4/std/string_fwd.hpp>
#include <c4/substr_fwd.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/parse.hpp>
#include <cstddef>

#include "LSChannel.h"
#include "LSMessageToken.h"
#include "LSTransport.h"

namespace c4 {
inline c4::substr to_substr(std::string& s) noexcept {
  return c4::substr{s.data(), s.size()};
}
inline c4::csubstr to_csubstr(const std::string& s) noexcept {
  return c4::csubstr{s.data(), s.size()};
}
}  // namespace c4

namespace vanadium::lserver {

template <typename TContextPayload>
class Server;

template <typename Payload>
class ServerContext {
 public:
  ServerContext(Server<Payload>* server);

  PooledMessageToken AcquireToken() {
    return pool_.Acquire();
  }
  void Send(PooledMessageToken&&);

  Payload& operator&() {
    return data_;
  }
  Payload* operator->() {
    return &data_;
  }

 private:
  Server<Payload>* const server_;
  TokenPool pool_;
  Payload data_;
};

template <typename TContextPayload>
class Server {
 public:
  using Context = ServerContext<TContextPayload>;
  using HandlerFn = void (*)(Context&, PooledMessageToken&&);

  Server(Transport& transport, HandlerFn handler, std::size_t concurrency, std::size_t backlog)
      : handler_(handler),
        channel_(transport, concurrency * 2),
        backlog_(backlog),
        task_arena_(concurrency),
        ctx_(this) {}

  void Listen() {
    is_running_ = true;

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
    task_arena_.execute([&] {
      for (int i = 0; i < task_arena_.max_concurrency(); ++i) {
        wg_.run([&] {
          while (is_running_.load()) {
            auto token = channel_.Poll();
            auto str = ryml::substr{token->buf.data(), token->buf.size()};
            ryml::parse_json_in_place(&token->parser, str, &token->tree);
            handler_(ctx_, std::move(token));
          }
        });
      }
    });

    wg_.wait();
  }

  void Stop() {
    is_running_ = false;
    wg_.cancel();
  }

  Context& GetContext() {
    return ctx_;
  }

  void Send(PooledMessageToken&& token) {
    ryml::emitrs_json(token->tree, &token->buf);
    channel_.Enqueue(std::move(token));
  }

  [[nodiscard]] std::size_t GetConcurrency() const noexcept {
    return task_arena_.max_concurrency();
  }
  [[nodiscard]] std::size_t GetBacklog() const noexcept {
    return backlog_;
  }

  Server(const Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(const Server&) = delete;
  Server& operator=(Server&&) = delete;

 private:
  HandlerFn handler_;

  Channel channel_;

  std::size_t backlog_;
  tbb::task_arena task_arena_;
  tbb::task_group wg_;

  std::atomic<bool> is_running_;

  Context ctx_;
};

template <typename Payload>
ServerContext<Payload>::ServerContext(Server<Payload>* server)
    : server_(server), pool_(server->GetConcurrency() * server->GetBacklog()) {}

template <typename Payload>
void ServerContext<Payload>::Send(PooledMessageToken&& token) {
  server_->Send(std::move(token));
}

}  // namespace vanadium::lserver
