#pragma once

#include <cstdint>        // IWYU pragma: export
#include <map>            // IWYU pragma: export
#include <optional>       // IWYU pragma: export
#include <string>         // IWYU pragma: export
#include <string_view>    // IWYU pragma: export
#include <tuple>          // IWYU pragma: export
#include <unordered_map>  // IWYU pragma: export
#include <variant>        // IWYU pragma: export
#include <vector>         // IWYU pragma: export

#include <glaze/json/generic_fwd.hpp>  // IWYU pragma: export

struct GlazeGenericStub {
  std::variant<std::nullptr_t, double, std::string, bool, std::vector<GlazeGenericStub>,
               std::map<std::string, GlazeGenericStub, std::less<> > >
      storage;

  virtual ~GlazeGenericStub() {}
  // like glz::generic, to match up the struct size
  // ^ glz::generic destructor is no longer virtual in the latest revision, remove this after updating glaze
};

using LSPAny = glz::generic;

using LSPArray = std::vector<LSPAny>;
using LSPObject = LSPAny;

struct LiteralStub {};  // TODO
