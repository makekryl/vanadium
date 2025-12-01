#pragma once

#include <oneapi/tbb/concurrent_queue.h>

#include "vanadium/lib/lserver/MessageToken.h"
#include "vanadium/lib/lserver/Transport.h"

namespace vanadium::lserver {

class Channel {
 public:
  Channel(Transport& transport, std::size_t tokens) : pool_(tokens), transport_(&transport) {}
  ~Channel() {}

  void Read();
  void Write();

  void Enqueue(PooledMessageToken&&);

  PooledMessageToken Poll();

 private:
  TokenPool pool_;
  Transport* transport_;

  tbb::concurrent_bounded_queue<PooledMessageToken> ready_;
  tbb::concurrent_bounded_queue<PooledMessageToken> out_queue_;
};

}  // namespace vanadium::lserver
