#pragma once

#include "Semantic.h"

namespace vanadium::core {

namespace builtins {

struct Superbases {
  const semantic::Symbol *kPort, *kClass;
};
extern Superbases superbases;

inline const semantic::Symbol* GetSuperbase(const semantic::Symbol* sym) {
  if (sym->Flags() & semantic::SymbolFlags::kPort) {
    return superbases.kPort;
  }
  return nullptr;
}
}  // namespace builtins

}  // namespace vanadium::core
