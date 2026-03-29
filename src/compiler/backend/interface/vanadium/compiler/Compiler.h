#pragma once

#include <vanadium/core/Program.h>

namespace vanadium::compiler {

struct CompileOptions {
  bool debug;
};

void CompileIR(const core::SourceFile&, const CompileOptions&);

}  // namespace vanadium::compiler
