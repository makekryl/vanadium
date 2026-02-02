#include "vanadium/bin/fmt/OptionsReader.h"

#include <string_view>

#include <glaze/toml/read.hpp>

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
  PartialManifest result;
  if (auto ec = glz::read<glz::opts{.format = glz::TOML, .error_on_unknown_keys = false}>(result, contents); ec) {
    return Error{glz::format_error(ec, contents)};
  }
  if (result.tools && result.tools->fmt) {
    const auto& c = *result.tools->fmt;
#define COPY_OPT_IF_PRESENT(OPT)        \
  do {                                  \
    if (const auto& opt = c.OPT; opt) { \
      opts.OPT = *opt;                  \
    }                                   \
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

}  // namespace vanadium::bin::fmt
