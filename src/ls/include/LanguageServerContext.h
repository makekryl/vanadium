#pragma once

#include <tbb/task_arena.h>

#include "LSConnection.h"
#include "Program.h"
#include "Project.h"

namespace vanadium::ls {

struct VanadiumLsState {
  tbb::task_arena task_arena;

  // tooling::Project project;
  core::Program program;
};

using VanadiumLsContext = lserver::ConnectionContext<VanadiumLsState>;
using VanadiumLsConnection = lserver::Connection<VanadiumLsState>;

}  // namespace vanadium::ls
