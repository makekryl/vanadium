#include <chrono>
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

  vanadium::lserver::StdioTransport::Setup();
  vanadium::lserver::StdioTransport transport;

  vanadium::ls::Serve(transport, 3, 1);

  return 0;
}
