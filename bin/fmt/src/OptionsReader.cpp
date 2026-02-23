#include "vanadium/bin/fmt/OptionsReader.h"

#include <string_view>

#include <rfl/toml/read.hpp>

#include <vanadium/format/AstPrinter.h>

namespace vanadium::bin::fmt {

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

std::optional<Error> TryReadOptionsFromManifest(std::string_view contents, format::PrintOptions& opts) {
  rfl::Result<PartialManifest> result = rfl::toml::read<PartialManifest>(contents);
  if (result.has_value()) {
    if (result->tools && result->tools->fmt) {
      const auto& c = *result->tools->fmt;
#define COPY_OPT_IF_PRESENT(OPT) \
  do {                           \
    if (c.OPT.has_value()) {     \
      opts.OPT = *c.OPT;         \
    }                            \
  } while (0)
      //
      COPY_OPT_IF_PRESENT(tab_width);
      COPY_OPT_IF_PRESENT(print_width);
      COPY_OPT_IF_PRESENT(max_empty_newlines);
      //
#undef COPY_OPT_IF_PRESENT
    }
    return std::nullopt;
  }
  return Error{result.error().what()};
}

}  // namespace vanadium::bin::fmt
