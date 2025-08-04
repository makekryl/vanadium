#include "Bootstrap.h"

#include <csignal>
#include <iostream>
#include <stacktrace>

namespace vanadium::bin {
EntryPoint* EntryPoint::list_{nullptr};
}

#ifndef NDEBUG
namespace {
void HandleSignal(int signum) {
  std::cerr << "--- Signal (" << signum << ") has been received ---" << std::endl;
  std::cerr << "--- STACKTRACE ---" << std::endl;
  std::cerr << std::stacktrace::current();
  std::cerr.flush();
  std::exit(signum);  // TODO: call original sighandler
}
}  // namespace
#endif

int main(int argc, char* argv[]) {
#ifndef NDEBUG
  std::signal(SIGABRT, HandleSignal);
  std::signal(SIGSEGV, HandleSignal);
#endif

  // TODO: support multiple entry points
  return vanadium::bin::EntryPoint::list_->invoke(argc, argv);
}
