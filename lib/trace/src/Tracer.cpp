#include "vanadium/lib/trace/Tracer.h"

#include <ranges>
#include <string>
#include <string_view>
#include <utility>

#include <vanadium/lib/FunctionRef.h>
#include <vanadium/lib/concurrency/ThreadSpecific.h>

namespace vanadium::lib::trace {

void Tracer::Serialize(lib::Consumer<std::string_view> write) const {
  auto write_events = [&, first = true](auto range, std::size_t off) mutable {
    for (const auto [tid, tracer] : range | std::views::enumerate) {
      const auto stid = std::to_string(off + tid);
      for (const auto& ev : tracer->events_) {
        if (!first) {
          write(",");
        }
        first = false;

        switch (ev.type) {
          case TraceEvent::Type::kComplete:
            write(R"({"name":")");
            write(ev.name);
            write(R"(","ph":"X","ts":)");
            write(std::to_string(ev.ts_us));
            write(R"(,"dur":)");
            write(std::to_string(ev.dur_us));
            write(R"(,"pid":1,"tid":)");
            write(stid);
            write("}");
            break;
          case TraceEvent::Type::kBegin:
            write(R"({"name":")");
            write(ev.name);
            write(R"(","ph":"B","ts":)");
            write(std::to_string(ev.ts_us));
            write(R"(,"pid":1,"tid":)");
            write(stid);
            write("}");
            break;
          case TraceEvent::Type::kEnd:
            write(R"({"ph":"E","ts":)");
            write(std::to_string(ev.ts_us));
            write(R"(,"pid":1,"tid":)");
            write(stid);
            write("}");
            break;
          default:
            std::unreachable();
        }
      }
    }
  };

  write(R"({"traceEvents":[)");
  write_events(std::views::single(&supervising_), 1);
  write_events(tracers_.All(), 2);
  write(R"(]})");
}

void Tracer::Clear() {
  supervising_.events_.clear();
  for (auto* tracer : tracers_.All()) {
    tracer->events_.clear();
  }
}

}  // namespace vanadium::lib::trace
