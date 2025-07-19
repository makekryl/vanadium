#include "Builtins.h"

#include "Semantic.h"
#include "StaticMap.h"

namespace vanadium::core {
namespace builtins {

const semantic::Symbol kAnytype{"anytype", nullptr, semantic::SymbolFlags::kBuiltin};
const semantic::Symbol kBoolean{"boolean", nullptr, semantic::SymbolFlags::kBuiltin};
const semantic::Symbol kInteger{"integer", nullptr, semantic::SymbolFlags::kBuiltin};
const semantic::Symbol kFloat{"float", nullptr, semantic::SymbolFlags::kBuiltin};
const semantic::Symbol kBitstring{"bitstring", nullptr, semantic::SymbolFlags::kBuiltin};
const semantic::Symbol kCharstring{"charstring", nullptr, semantic::SymbolFlags::kBuiltin};
const semantic::Symbol kOctetstring{"octetstring", nullptr, semantic::SymbolFlags::kBuiltin};
const semantic::Symbol kHexstring{"hexstring", nullptr, semantic::SymbolFlags::kBuiltin};
const semantic::Symbol kUniversalCharstring{"universal charstring", nullptr, semantic::SymbolFlags::kBuiltin};
const semantic::Symbol kVerdictType{"verdicttype", nullptr, semantic::SymbolFlags::kBuiltin};

const semantic::Symbol* ResolveBuiltin(std::string_view name) {
  static constexpr lib::StaticMap<std::string_view, const semantic::Symbol*, 10> kBuiltinsTable{{{
      {"anytype", &kAnytype},
      {"boolean", &kBoolean},
      {"integer", &kInteger},
      {"float", &kFloat},
      {"bitstring", &kBitstring},
      {"charstring", &kCharstring},
      {"octetstring", &kOctetstring},
      {"hexstring", &kHexstring},
      {"universal charstring", &kUniversalCharstring},
      {"verdicttype", &kVerdictType},
  }}};
  if (const auto sym_opt = kBuiltinsTable.get(name); sym_opt) {
    return *sym_opt;
  }
  return nullptr;
}

}  // namespace builtins
}  // namespace vanadium::core
