#pragma once

#include <span>
#include <string>

#include <vanadium/lib/FunctionRef.h>

namespace vanadium::tooling {
void SetCompilerExtensions(std::span<const std::string> extensions,
                           lib::Consumer<const std::string&> on_unknown_extension);
}
