#pragma once

#include <string_view>

namespace vanadium::core {

namespace semantic {
class Symbol;
}

namespace builtins {
extern const semantic::Symbol kAnytype;
extern const semantic::Symbol kBoolean;
extern const semantic::Symbol kInteger;
extern const semantic::Symbol kFloat;
extern const semantic::Symbol kBitstring;
extern const semantic::Symbol kCharstring;
extern const semantic::Symbol kOctetstring;
extern const semantic::Symbol kHexstring;
extern const semantic::Symbol kUniversalCharstring;
extern const semantic::Symbol kVerdictType;

const semantic::Symbol* ResolveBuiltin(std::string_view name);
}  // namespace builtins

}  // namespace vanadium::core
