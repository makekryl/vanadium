#pragma once

#include <llvm/IR/Module.h>

namespace vanadium::compiler {

void ConfigureTargetTriple(llvm::Module&);

}
