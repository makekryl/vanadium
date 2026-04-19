#include <span>

#include "vanadium/runtime/runtime.h"

namespace vanadium::hostc {
[[nodiscard]] std::span<const vrt_module_t* const> GetModules();
}
