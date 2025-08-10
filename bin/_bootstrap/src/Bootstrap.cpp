#include "Bootstrap.h"

#include <csignal>
#include <iostream>
#include <print>
#include <sstream>
#include <stacktrace>

namespace vanadium::bin {
EntryPoint* EntryPoint::list_{nullptr};
}

#ifndef NDEBUG
namespace {
void HandleSignal(int signum) {
  std::stringstream ss;
  ss << "--- Signal (" << signum << ") has been received ---" << std::endl;
  ss << "--- STACKTRACE ---" << std::endl;
  ss << std::stacktrace::current();

  const auto str = ss.str();
  std::println(stderr, "\n{}\n", str);
  std::fflush(stderr);

  std::signal(signum, SIG_DFL);
  std::raise(signum);
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
