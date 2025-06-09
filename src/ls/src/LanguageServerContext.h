#pragma once

#include <oneapi/tbb/task_arena.h>

#include <cstddef>

#include "LSServer.h"

namespace vanadium::ls {

struct VanadiumLsState {
  tbb::task_arena task_arena;
};

using VanadiumLsContext = lserver::ServerContext<VanadiumLsState>;

}  // namespace vanadium::ls
