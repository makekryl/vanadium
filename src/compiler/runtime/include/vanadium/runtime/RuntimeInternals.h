#include <span>

#include "vanadium/runtime/runtime.h"

namespace vanadium::rt {
[[nodiscard]] std::span<const vrt_module_t* const> GetModules();
}
