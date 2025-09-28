#include <argparse/argparse.hpp>
#include <chrono>
#include <csignal>
#include <fstream>
#include <iostream>
#include <print>
#include <string_view>
#include <thread>

#include "Bootstrap.h"
#include "LSTransport.h"
#include "LanguageServer.h"

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
  ap.add_argument("-j", "--parallel", "").store_into(jobs).help("maximum number of worker threads");
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

  /////////////////////////////////////////////
  try {
    ap.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << ap;
    return 1;
  }
  /////////////////////////////////////////////

  if (ap["--wait-dbg"] == true) {  // == true, yes
    constexpr auto kSecondsToWait{12};
    std::println(stderr, "\nWaiting {} seconds for debugger to attach...\n", kSecondsToWait);
    std::this_thread::sleep_for(std::chrono::seconds(kSecondsToWait));
  }

  vanadium::lserver::StdioTransport::Setup();
  vanadium::lserver::StdioTransport stdio_transport;

  vanadium::lserver::Transport* transport{&stdio_transport};

  std::optional<OutputCapturingTransport> capturing_transport;
  if (ap["--capture-output"] == true) {
    transport = &capturing_transport.emplace("vanadiumd.stdout.log", *transport);
  }

  vanadium::ls::Serve(*transport, concurrency, jobs);

  return 0;
}
}  // namespace

DEFINE_VANADIUM_ENTRYPOINT(main);
