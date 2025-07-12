#include <chrono>
#include <csignal>
#include <iostream>
#include <stacktrace>
#include <thread>

#include "LSTransport.h"
#include "LanguageServer.h"

namespace {
void WaitDebugger() {
  volatile bool dbg_attached = false;
  while (!dbg_attached) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}
}  // namespace

int main() {
  // WaitDebugger();

  std::signal(SIGABRT, [](int signum) {
    std::cerr << "STACKTRACE:" << std::endl;
    std::cerr << std::stacktrace::current();
    std::cerr.flush();
    std::exit(signum);
  });

  vanadium::lserver::StdioTransport::Setup();
  vanadium::lserver::StdioTransport transport;

  vanadium::ls::Serve(transport, 3, 4);

  return 0;
}
