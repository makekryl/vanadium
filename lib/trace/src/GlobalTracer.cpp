#include "vanadium/lib/trace/GlobalTracer.h"

namespace vanadium::trace {
lib::trace::OptionalTracer global{};
std::string global_save_path{"vanadium.trace.json"};
}  // namespace vanadium::trace
