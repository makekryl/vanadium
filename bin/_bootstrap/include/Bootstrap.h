#pragma once

#include <utility>

namespace vanadium::bin {
class EntryPoint {
 public:
  using Fn = int (*)(int argc, char* argv[]);

  EntryPoint(Fn invoke) : invoke_(invoke), next_(std::exchange(chain_, this)) {}

  int invoke(int argc, char* argv[]) {
    return invoke_(argc, argv);
  }

 private:
  Fn invoke_;
  EntryPoint* next_;

 public:
  static EntryPoint* chain_;
};
}  // namespace vanadium::bin
