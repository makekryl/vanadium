#pragma once

#include <vanadium/lib/concurrency/ConcurrentQueue.h>

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

  lib::concurrency::ConcurrentQueue<PooledMessageToken> ready_;
  lib::concurrency::ConcurrentQueue<PooledMessageToken> out_queue_;
};

}  // namespace vanadium::lserver
