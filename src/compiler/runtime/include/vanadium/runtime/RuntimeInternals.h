#include <span>

#include "vanadium/runtime/runtime.h"  // IWYU pragma: export

namespace vanadium::rt {
[[nodiscard]] std::span<const vrt_module_t* const> GetModules();
}
