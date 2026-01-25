#pragma once

#include <rfl/toml/read.hpp>

#include "vanadium/tooling/Project.h"

// ReadSpec is extracted to this header to avoid polluting all dependents of Project.h by reflectcpp headers

namespace vanadium::tooling {
template <typename T>
[[nodiscard]] std::expected<T, Error> Project::ReadSpec() const {
  rfl::Result<T> result = rfl::toml::read<T>(manifest_contents_);
  if (result.has_value()) {
    return result.value();
  }
  return Error{result.error().what()};
}
}  // namespace vanadium::tooling
