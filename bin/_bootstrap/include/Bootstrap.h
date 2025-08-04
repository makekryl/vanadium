#pragma once

#include <utility>

namespace vanadium::bin {
class EntryPoint {
 public:
  using Fn = int (*)(int argc, char* argv[]);

  EntryPoint(Fn invoke) : invoke_(invoke), next_(std::exchange(list_, this)) {}

  int invoke(int argc, char* argv[]) {
    return invoke_(argc, argv);
  }

 private:
  Fn invoke_;
  EntryPoint* next_;

 public:
  static EntryPoint* list_;
};
}  // namespace vanadium::bin

#define DEFINE_VANADIUM_ENTRYPOINT(fn)  \
  namespace {                           \
  vanadium::bin::EntryPoint vnep__(fn); \
  }
