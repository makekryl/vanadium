#pragma once

#include <optional>
#include <string_view>

#include <vanadium/format/AstPrinter.h>
#include <vanadium/lib/Error.h>

namespace vanadium::bin::fmt {
std::optional<Error> TryReadOptionsFromManifest(std::string_view contents, format::PrintOptions&);
}
