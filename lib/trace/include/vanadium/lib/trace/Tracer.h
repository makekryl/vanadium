#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <vanadium/lib/FunctionRef.h>
#include <vanadium/lib/concurrency/ThreadSpecific.h>

namespace vanadium::lib::trace {

inline std::uint64_t TimestampUs() {
  const auto now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

struct TraceEvent {
  enum class Type : std::uint8_t { kBegin, kEnd, kComplete };

  std::string name;
  std::uint64_t ts_us;
  std::uint64_t dur_us;
  Type type;
};

class ThreadTracer {
 public:
  class ScopeGuard {
   public:
    ScopeGuard(const ThreadTracer&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

    ScopeGuard(ScopeGuard&& other) noexcept : tracer_{std::exchange(other.tracer_, nullptr)} {}

    ~ScopeGuard() {
      if (tracer_) {
        tracer_->End();
      }
    }

    // TODO: this was intended to reduce git diff and nesting in Program::Analyze
    static void Transit(std::optional<ThreadTracer::ScopeGuard>& v, auto f) {
      v.reset();
      if (auto t = f()) {
        v.emplace(ScopeGuard{*std::exchange(t->tracer_, nullptr)});
      }
    }

   private:
    ScopeGuard(ThreadTracer& tracer) : tracer_{&tracer} {}

    ThreadTracer* tracer_;
    friend ThreadTracer;
  };

  void Complete(std::string_view name, std::uint64_t start_us, std::uint64_t dur_us) {
    events_.push_back({
        .name = std::string{name},
        .ts_us = start_us,
        .dur_us = dur_us,
        .type = TraceEvent::Type::kComplete,
    });
  }

  ScopeGuard Scope(std::string_view name) {
    Begin(name);
    return {*this};
  }

  void Begin(std::string_view name) {
    events_.push_back({
        .name = std::string{name},
        .ts_us = TimestampUs(),
        .dur_us = 0,
        .type = TraceEvent::Type::kBegin,
    });
  }

  void End() {
    events_.push_back({
        .name = {},
        .ts_us = TimestampUs(),
        .dur_us = 0,
        .type = TraceEvent::Type::kEnd,
    });
  }

 private:
  std::vector<TraceEvent> events_;

  friend class Tracer;
};

class Tracer {
 public:
  ThreadTracer& Supervising() {
    return supervising_;
  }
  ThreadTracer& Local() {
    return tracers_.Local();
  }

  ThreadTracer* operator->() {
    return &Local();
  }

  void Serialize(lib::Consumer<std::string_view> write) const;
  void Clear();

 private:
  ThreadTracer supervising_;
  concurrency::ThreadSpecific<ThreadTracer> tracers_;
};

}  // namespace vanadium::lib::trace
