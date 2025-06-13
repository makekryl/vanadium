#pragma once

#include <tbb/task_arena.h>

#include <cstddef>

#include "LSServer.h"
#include "Program.h"
#include "Project.h"

namespace vanadium::ls {

struct VanadiumLsState {
  tbb::task_arena task_arena;

  // tooling::Project project;
  core::Program program;
};

using VanadiumLsContext = lserver::ServerContext<VanadiumLsState>;
using VanadiumLsServer = lserver::Server<VanadiumLsState>;

}  // namespace vanadium::ls
