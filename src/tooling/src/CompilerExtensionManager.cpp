#include "vanadium/tooling/CompilerExtensionManager.h"

#include <ranges>

#include <vanadium/lib/StaticMap.h>

#define DEFINE_COMPILER_EXTENSION(ns, name) \
  namespace ns::compilerExtensions {        \
  extern bool name;                         \
  }
//
DEFINE_COMPILER_EXTENSION(vanadium::asn1::ast, eag_grouping)
//
#undef DEFINE_COMPILER_EXTENSION

namespace vanadium::tooling {

namespace {
constexpr auto kCompilerExtensions = lib::MakeStaticMap<std::string_view, bool*>({
    {"asn1-eag-grouping", &vanadium::asn1::ast::compilerExtensions::eag_grouping},
});
}

void SetCompilerExtensions(std::span<const std::string> extensions,
                           lib::Consumer<const std::string&> on_unknown_extension) {
  for (bool* flag : kCompilerExtensions.Entries() | std::views::values) {
    *flag = false;
  }

  for (const auto& ext : extensions) {
    if (auto flag_opt = kCompilerExtensions.get(ext); flag_opt) {
      *(*flag_opt) = true;
    } else {
      on_unknown_extension(ext);
    }
  }
}

}  // namespace vanadium::tooling
