#pragma once

#include <optional>
#include <string>

#include "vanadium/lib/trace/Tracer.h"

namespace vanadium::lib::trace {

class OptionalThreadTracer {
 public:
  OptionalThreadTracer* operator->() {
    return this;
  }

  void Complete(std::string_view name, std::uint64_t start_us, std::uint64_t dur_us) {
    if (tracer_) {
      tracer_->Complete(name, start_us, dur_us);
    }
  }

  std::optional<ThreadTracer::ScopeGuard> Scope(std::string_view name) {
    if (!tracer_) {
      return std::nullopt;
    }
    return tracer_->Scope(name);
  }

  void Begin(std::string_view name) {
    if (tracer_) {
      tracer_->Begin(name);
    }
  }

  void End() {
    if (tracer_) {
      tracer_->End();
    }
  }

 private:
  OptionalThreadTracer(ThreadTracer* tracer) : tracer_{tracer} {}

  ThreadTracer* tracer_;
  friend class OptionalTracer;
};

class OptionalTracer {
 public:
  OptionalThreadTracer Supervising() {
    if (!tracer_) {
      return {nullptr};
    }
    return {&tracer_->Supervising()};
  }
  OptionalThreadTracer Local() {
    if (!tracer_) {
      return {nullptr};
    }
    auto* tracer = &tracer_->Local();
    [[assume(tracer != nullptr)]];  // gcc can't figure it on its own, https://godbolt.org/z/9E3TYjaoP
    return {tracer};
  }

  OptionalThreadTracer operator->() {
    return Local();
  }

  std::optional<Tracer>& Unwrap() {
    return tracer_;
  }
  [[nodiscard]] bool Active() {
    return tracer_.has_value();
  }
  void Setup() {
    tracer_.emplace();
  }
  void Teardown() {
    tracer_.reset();
  }

 private:
  std::optional<Tracer> tracer_;
};

}  // namespace vanadium::lib::trace

namespace vanadium::trace {
extern lib::trace::OptionalTracer global;
extern std::string global_save_path;
}  // namespace vanadium::trace
