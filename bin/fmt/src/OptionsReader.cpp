#include "vanadium/bin/fmt/OptionsReader.h"

#include <string_view>

#include <glaze/toml/read.hpp>

#include <vanadium/format/AstPrinter.h>

namespace vanadium::bin::fmt {

// NOLINTBEGIN(misc-use-internal-linkage): glaze; remove after C++26?
struct PartialToolsSection {
  std::optional<format::PrintOptions> fmt;
};
struct PartialManifest {
  std::optional<PartialToolsSection> tools;
};
// NOLINTEND(misc-use-internal-linkage)

std::optional<Error> TryReadOptionsFromManifest(std::string_view contents, format::PrintOptions& opts) {
  PartialManifest manifest{
      .tools = PartialToolsSection{.fmt = opts},
  };

  if (auto ec = glz::read<glz::opts{.format = glz::TOML, .error_on_unknown_keys = false}>(manifest, contents); ec) {
    return Error{glz::format_error(ec, contents)};
  }

  if (const auto& v = manifest.tools->fmt) {
    opts = *v;
  }
  return std::nullopt;
}

}  // namespace vanadium::bin::fmt
