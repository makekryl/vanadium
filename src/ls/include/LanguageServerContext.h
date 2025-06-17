#pragma once

#include <tbb/enumerable_thread_specific.h>
#include <tbb/task_arena.h>

#include "Arena.h"
#include "LSConnection.h"
#include "Linter.h"
#include "Program.h"
#include "Project.h"
#include "oneapi/tbb/enumerable_thread_specific.h"

namespace vanadium::ls {

struct VanadiumLsState {
  tbb::task_arena task_arena;

  std::optional<tooling::Project> project;
  core::Program program;

  lint::Linter linter;

  lib::Arena& GetTemporaryArena() {
    return temporary_arena_.local();
  }

 private:
  tbb::enumerable_thread_specific<lib::Arena> temporary_arena_;
};

using LsContext = lserver::ConnectionContext<VanadiumLsState>;
using VanadiumLsConnection = lserver::Connection<VanadiumLsState>;

}  // namespace vanadium::ls
