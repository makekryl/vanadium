#pragma once

#include <chrono>
#include <print>

namespace vanadium::lib {

template <typename TDur = std::chrono::milliseconds>
class TimeTracker {
 public:
  TimeTracker(std::string prefix) : prefix_(std::move(prefix)), ts_begin_(Now()) {}

  void Measure(std::string_view prefix) {
    Log(prefix);
    ts_begin_ = Now();
  }

  ~TimeTracker() {
    Log(prefix_);
  }

 private:
  void Log(std::string_view prefix) {
    const auto ts_end = Now();
    const auto elapsed = ts_end - ts_begin_;

    if (std::chrono::duration_cast<TDur>(elapsed).count() < 2) {
      return;
    }

    std::println(stderr, " [{}]: elapsed {:%Q%q}", prefix, std::chrono::duration_cast<TDur>(elapsed));
  }

  std::string prefix_;

  using Clock = std::chrono::steady_clock;

  static Clock::time_point Now() {
    return Clock::now();
  }

  Clock::time_point ts_begin_;
};

}  // namespace vanadium::lib
