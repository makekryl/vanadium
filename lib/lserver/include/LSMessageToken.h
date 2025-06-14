#pragma once

#include <oneapi/tbb/concurrent_queue.h>

#include <utility>
#include <vector>

namespace vanadium::lserver {

struct MessageToken {
  using buffer_t = std::string;

  buffer_t buf;
};

class TokenPool;

class PooledMessageToken {
 public:
  PooledMessageToken() = default;
  PooledMessageToken(MessageToken* token, TokenPool* pool) : token_(token), pool_(pool) {}

  ~PooledMessageToken();

  MessageToken& operator*() {
    return *token_;
  }
  MessageToken* operator->() {
    return token_;
  }

  PooledMessageToken(const PooledMessageToken&) = delete;
  PooledMessageToken& operator=(const PooledMessageToken&) = delete;

  PooledMessageToken(PooledMessageToken&& other) noexcept
      : token_(std::exchange(other.token_, nullptr)), pool_(std::exchange(other.pool_, nullptr)) {};
  PooledMessageToken& operator=(PooledMessageToken&& other) noexcept {
    std::swap(token_, other.token_);
    std::swap(pool_, other.pool_);
    return *this;
  }

 private:
  MessageToken* token_{nullptr};
  TokenPool* pool_{nullptr};
};

class TokenPool {
 public:
  TokenPool(std::size_t capacity) : storage_(capacity) {
    for (auto& token : storage_) {
      Release(PooledMessageToken{&token, this});
    }
  }

  PooledMessageToken Acquire() {
    PooledMessageToken result;
    pool_.pop(result);

    result->buf.clear();

    return result;
  }

  void Release(PooledMessageToken&& token) {
    pool_.emplace(std::move(token));
  }

  [[nodiscard]] bool Full() noexcept {
    return pool_.empty();
  }

 private:
  std::vector<MessageToken> storage_;
  tbb::concurrent_bounded_queue<PooledMessageToken> pool_;
};

inline PooledMessageToken::~PooledMessageToken() {
  if (pool_ == nullptr) {
    return;
  }
  pool_->Release(std::move(*this));
}

}  // namespace vanadium::lserver
