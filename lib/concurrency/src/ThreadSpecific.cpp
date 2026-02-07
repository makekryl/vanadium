#include "vanadium/lib/concurrency/ThreadSpecific.h"

#include <mutex>
#include <utility>

#include <pthread.h>

namespace vanadium::lib::concurrency {

namespace {
// TODO: Windows support
std::uint32_t TSCreateKey() {
  static_assert(sizeof(std::uint32_t) == sizeof(pthread_key_t));
  std::uint32_t key;
  pthread_key_create(&key, nullptr);
  return key;
}
void TSDestroyKey(GenericThreadSpecific::Key key) {
  pthread_key_delete(key);
}
void TSSetLocal(GenericThreadSpecific::Key key, void* value) {
  pthread_setspecific(key, value);
}
void* TSGetLocal(GenericThreadSpecific::Key key) {
  return pthread_getspecific(key);
}
}  // namespace

GenericThreadSpecific::GenericThreadSpecific(AllocFunc alloc, DeallocFunc dealloc)
    : key_(TSCreateKey()), alloc_(std::move(alloc)), dealloc_(std::move(dealloc)) {}

GenericThreadSpecific::~GenericThreadSpecific() {
  TSDestroyKey(key_);

  for (void* p : allocated_) {
    dealloc_(p);
  }
}

void* GenericThreadSpecific::Local() const {
  if (void* p = TSGetLocal(key_); p) [[likely]] {
    return p;
  }

  void* p = alloc_();
  TSSetLocal(key_, p);
  {
    std::lock_guard lock(mtx_);
    allocated_.emplace_back(p);
  }
  return p;
}

}  // namespace vanadium::lib::concurrency
