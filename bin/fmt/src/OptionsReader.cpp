#include "vanadium/bin/fmt/OptionsReader.h"

#include <string_view>

#include <rfl/toml/read.hpp>

#include <vanadium/format/AstPrinter.h>

namespace vanadium::bin::fmt {

struct PartialPrintOptions {
  rfl::DefaultVal<std::size_t> tab_width{4};
  rfl::DefaultVal<std::size_t> print_width{80};

  rfl::DefaultVal<std::size_t> max_empty_newlines{2};
};

struct PartialToolsSection {
  std::optional<PartialPrintOptions> fmt;
};

struct PartialManifest {
  std::optional<PartialToolsSection> tools;
};

std::optional<Error> TryReadOptionsFromManifest(std::string_view contents, format::PrintOptions& opts) {
  rfl::Result<PartialManifest> result = rfl::toml::read<PartialManifest>(contents);
  if (result.has_value()) {
    if (result->tools && result->tools->fmt) {
      const auto& c = *result->tools->fmt;
      opts.tab_width = c.tab_width.value();
      opts.print_width = c.print_width.value();
      opts.max_empty_newlines = c.max_empty_newlines.value();
    }
    return std::nullopt;
  }
  return Error{result.error().what()};
}

}  // namespace vanadium::bin::fmt
