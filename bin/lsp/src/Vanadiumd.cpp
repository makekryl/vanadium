#include <vanadium/bin/Bootstrap.h>
#include <vanadium/lib/lserver/Transport.h>
#include <vanadium/version.h>

#include <argparse/argparse.hpp>
#include <chrono>
#include <csignal>
#include <fstream>
#include <iostream>
#include <print>
#include <string_view>
#include <thread>

#include "LanguageServer.h"
#include "LanguageServerTestFlags.h"

class OutputCapturingTransport : public vanadium::lserver::Transport {
 public:
  OutputCapturingTransport(const char* filename, vanadium::lserver::Transport& base) : base_(base) {
    flog_ = std::ofstream(filename, std::ios::trunc);
  }

  void Read(std::span<char> chunk) final {
    base_.Read(chunk);
  }
  void ReadLine(std::span<char> chunk) final {
    base_.ReadLine(chunk);
  }
  void Flush() final {
    base_.Flush();
  }

  void Write(std::string_view buf) final {
    flog_ << buf;
    flog_ << "\n---\n";
    flog_.flush();
    //
    base_.Write(buf);
  }

 private:
  std::ofstream flog_;
  vanadium::lserver::Transport& base_;
};

namespace {
int main(int argc, char* argv[]) {
  argparse::ArgumentParser ap("vanadiumd");
  ap.add_description("TTCN-3 language server");
  //
  std::uint32_t jobs{std::clamp(std::thread::hardware_concurrency(), 1U, 4U)};
  ap.add_argument("-j", "--jobs", "").store_into(jobs).help("maximum number of worker threads");
  std::uint32_t concurrency{1};  // >1 is not supported at the moment
  ap.add_argument("--concurrency").store_into(concurrency).help("number of concurrently served LS requests");
  //
  {
    auto& transport_group = ap.add_mutually_exclusive_group();
    transport_group.add_argument("--stdio").help("run LSP over standard IO").flag();
  }
  //
  ap.add_argument("--wait-dbg").flag();
  ap.add_argument("--capture-output").flag();
  ap.add_argument("--full-analysis").flag();

  //
  PARSE_CLI_ARGS_OR_EXIT(ap, argc, argv, 1);
  //

  std::println(stderr, "vanadiumd version {}", vanadium::bin::kVersion);

  if (ap.get<bool>("--wait-dbg")) {
    constexpr auto kSecondsToWait{12};
    std::println(stderr, "\nWaiting {} seconds for debugger to attach...\n", kSecondsToWait);
    std::this_thread::sleep_for(std::chrono::seconds(kSecondsToWait));
  }

  if (ap.get<bool>("--full-analysis")) {
    std::println(stderr, "\nWARN: Full analysis mode is enabled\n");
    vanadium::ls::testflags::do_not_skip_full_analysis = true;
  }

  vanadium::lserver::StdioTransport::Setup();
  vanadium::lserver::StdioTransport stdio_transport;

  vanadium::lserver::Transport* transport{&stdio_transport};

  std::optional<OutputCapturingTransport> capturing_transport;
  if (ap.get<bool>("--capture-output")) {
    transport = &capturing_transport.emplace("vanadiumd.stdout.log", *transport);
  }

  vanadium::ls::Serve(*transport, concurrency, jobs);

  return 0;
}
}  // namespace

DEFINE_VANADIUM_ENTRYPOINT(main);
