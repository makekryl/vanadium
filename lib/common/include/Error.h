#pragma once

#include <memory>
#include <optional>
#include <string>

class Error {
 public:
  explicit Error(std::string what) : what_(std::move(what)) {}
  Error(std::string what, Error&& cause) : what_(std::move(what)), cause_(std::make_unique<Error>(std::move(cause))) {}

  [[nodiscard]] const std::string& What() const {
    return what_;
  }

  [[nodiscard]] std::optional<Error*> Cause() const {
    if (!cause_) {
      return std::nullopt;
    }
    return cause_.get();
  }

  [[nodiscard]] std::string String() const {
    std::string buf;
    std::size_t shift{0};
    for (const auto* err = this; err != nullptr; err = err->cause_.get()) {
      buf += err->what_;
      if (err->cause_) {
        ++shift;
        buf += "\n";
        for (std::size_t s = 0; s < shift; s++) {
          buf += "  ";
        }
        buf += "Cause: ";
      }
    }
    return buf;
  }

 private:
  std::string what_;
  std::unique_ptr<Error> cause_;
};
