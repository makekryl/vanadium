#pragma once

#include <span>
#include <string_view>

namespace vanadium::lserver {

class Transport {
 public:
  virtual ~Transport() = default;
  virtual void Read(std::span<char> chunk) = 0;
  virtual void ReadLine(std::span<char> chunk) = 0;
  virtual void Write(std::string_view) = 0;
  virtual void Flush() = 0;
};

class StdioTransport : public Transport {
 public:
  void Read(std::span<char> chunk) final;
  void ReadLine(std::span<char> chunk) final;
  void Write(std::string_view) final;
  void Flush() final;

  static void Setup();
};

}  // namespace vanadium::lserver
