#pragma once

#include <cstddef>
#include <expected>
#include <optional>

#include <vanadium/lib/Error.h>

namespace vanadium {

namespace tooling {
class Project;
}

namespace ls::tools {

struct PartialPrintOptions {
  std::optional<std::size_t> tab_width;
  std::optional<std::size_t> print_width;

  std::optional<std::size_t> max_empty_newlines;
};

struct PartialToolsSection {
  std::optional<PartialPrintOptions> fmt;
};

struct PartialManifest {
  std::optional<PartialToolsSection> tools;
};

std::expected<PartialManifest, Error> ReadPartialManifest(const tooling::Project&);

}  // namespace ls::tools

}  // namespace vanadium
