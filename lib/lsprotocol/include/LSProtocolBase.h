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

struct GlazeGenericStub {
  std::variant<std::nullptr_t, double, std::string, bool, std::vector<GlazeGenericStub>,
               std::map<std::string, GlazeGenericStub, std::less<> > >
      storage;

  virtual ~GlazeGenericStub() {}
  // like glz::generic, to match up the struct size
  // ^ glz::generic destructor is no longer virtual in the latest revision, remove this after updating glaze
};

#ifdef LSPROTOCOL_CODEC
using LSPAny = glz::generic;
static_assert(sizeof(GlazeGenericStub) == sizeof(glz::generic));
#else
using LSPAny = GlazeGenericStub;
#endif

using LSPArray = std::vector<LSPAny>;
using LSPObject = LSPAny;

struct LiteralStub {};  // TODO
