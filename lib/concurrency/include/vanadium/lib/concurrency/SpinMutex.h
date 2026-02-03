#pragma once

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#define V_SPINMUTEX__HAS_MM_PAUSE
#endif

#include <atomic>
#include <thread>
#ifdef V_SPINMUTEX__HAS_MM_PAUSE
#include <immintrin.h>  // _mm_pause
#endif

namespace vanadium::lib::concurrency {

class SpinMutex {
 public:
  SpinMutex() = default;

  SpinMutex(const SpinMutex&) = delete;
  SpinMutex(SpinMutex&&) = delete;
  SpinMutex& operator=(const SpinMutex&) = delete;
  SpinMutex& operator=(SpinMutex&&) = delete;

  void lock() noexcept {
    if (!flag_.test_and_set(std::memory_order_acquire)) {
      return;
    }

    while (true) {
      // see tbb's atomic_backoff::pause
      static constexpr std::int32_t kLoopsBeforeYield = 16;
      std::int32_t count{1};
      while (flag_.test(std::memory_order_relaxed)) {
        if (count <= kLoopsBeforeYield) {
          cpu_relax();
          count *= 2;
        } else {
          std::this_thread::yield();
        }
      }

      // Try again
      if (!flag_.test_and_set(std::memory_order_acquire)) {
        return;
      }
    }
  }

  void unlock() noexcept {
    flag_.clear(std::memory_order_release);
  }

  bool try_lock() noexcept {
    return !flag_.test_and_set(std::memory_order_acquire);
  }

 private:
  std::atomic_flag flag_ = ATOMIC_FLAG_INIT;

  static void cpu_relax() noexcept {
#ifdef V_SPINMUTEX__HAS_MM_PAUSE
    _mm_pause();
#elif defined(__aarch64__) || defined(__arm__)
    // TODO: check whether it actually works
    // TBB uses __asm__ __volatile__("isb sy" ::: "memory");
    asm volatile("yield");
#else
    std::this_thread::yield();
#endif
  }
};

}  // namespace vanadium::lib::concurrency

#undef V_SPINMUTEX__HAS_MM_PAUSE
