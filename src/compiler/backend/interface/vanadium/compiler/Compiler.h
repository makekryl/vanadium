#pragma once

#include <vanadium/core/Program.h>
#include <vanadium/lib/FunctionRef.h>

namespace llvm {
class Module;
}

namespace vanadium::compiler {

struct CompileOptions {
  bool debug;
};

void Compile(const core::Program&, const CompileOptions&, lib::Consumer<const core::SourceFile&, llvm::Module&>);

}  // namespace vanadium::compiler
