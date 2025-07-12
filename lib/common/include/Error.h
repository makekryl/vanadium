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

 private:
  std::string what_;
  std::unique_ptr<Error> cause_;
};
