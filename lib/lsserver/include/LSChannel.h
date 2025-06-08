#pragma once

#include <oneapi/tbb/concurrent_queue.h>

#include "LSMessageToken.h"
#include "LSTransport.h"

namespace vanadium::lsp {

class Channel {
 public:
  Channel(TokenPool& pool, Transport& transport) : pool_(&pool), transport_(&transport) {}
  ~Channel() {}

  void Read();
  void Write();

  void Enqueue(PooledMessageToken&&);

  PooledMessageToken Poll();

 private:
  TokenPool* pool_;
  Transport* transport_;

  tbb::concurrent_bounded_queue<PooledMessageToken> ready_;
  tbb::concurrent_bounded_queue<PooledMessageToken> out_queue_;
};

}  // namespace vanadium::lsp
