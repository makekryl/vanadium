#pragma once

#include "vanadium/core/Semantic.h"

namespace vanadium::core {

namespace builtins {

struct Superbases {
  const semantic::Symbol *kPort, *kComponent;
};
extern Superbases superbases;

inline const semantic::Symbol* GetSuperbase(const semantic::Symbol* sym) {
  if (sym->Flags() & semantic::SymbolFlags::kPort) {
    return superbases.kPort;
  }
  if (sym->Flags() & semantic::SymbolFlags::kComponent) {
    return superbases.kComponent;
  }
  return nullptr;
}
}  // namespace builtins

}  // namespace vanadium::core
