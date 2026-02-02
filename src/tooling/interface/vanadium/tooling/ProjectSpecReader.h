#pragma once

#include <glaze/core/reflect.hpp>
#include <glaze/toml.hpp>

#include "vanadium/tooling/Project.h"

// ReadSpec is extracted to this header to avoid polluting all dependents of Project.h by reflectcpp headers

namespace vanadium::tooling {
template <typename T>
[[nodiscard]] std::expected<T, Error> Project::ReadSpec() const {
  T obj;
  if (auto ec = glz::read<glz::opts{.format = glz::TOML, .error_on_unknown_keys = false}>(obj, manifest_contents_);
      ec) {
    return std::unexpected{Error{glz::format_error(ec, manifest_contents_)}};
  }
  return obj;
}
}  // namespace vanadium::tooling
