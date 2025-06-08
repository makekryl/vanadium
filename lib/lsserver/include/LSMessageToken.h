#pragma once

#include <oneapi/tbb/concurrent_queue.h>

#include <ryml.hpp>
#include <vector>

namespace vanadium::lsp {

struct MessageToken {
  using buffer_t = std::vector<char>;

  buffer_t buf;

  ryml::Parser parser{&ryml_evt_handler_};
  ryml::Tree tree;

 private:
  ryml::EventHandlerTree ryml_evt_handler_;
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

 private:
  MessageToken* token_;
  TokenPool* pool_;
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
    result->tree.clear();

    return result;
  }

  void Release(PooledMessageToken&& token) {
    pool_.emplace(token);
  }

  [[nodiscard]] bool Full() noexcept {
    return pool_.empty();
  }

 private:
  std::vector<MessageToken> storage_;
  tbb::concurrent_bounded_queue<PooledMessageToken> pool_;
};

inline PooledMessageToken::~PooledMessageToken() {
  // pool_->Release(std::move(*this));
}

}  // namespace vanadium::lsp
