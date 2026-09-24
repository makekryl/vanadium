#pragma once

#include <expected>
#include <optional>

#include <vanadium/lib/Error.h>

#include "vanadium/format/AstPrinter.h"

namespace vanadium {

namespace tooling {
class Project;
}

namespace ls::tools {

struct PartialToolsSection {
  std::optional<format::PrintOptions> fmt;
};

struct PartialManifest {
  std::optional<PartialToolsSection> tools;
};

std::expected<PartialManifest, Error> ReadPartialManifest(const tooling::Project&);

}  // namespace ls::tools

}  // namespace vanadium
