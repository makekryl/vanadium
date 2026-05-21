#pragma once

#include <string>
#include <string_view>

#include <vanadium/ast/ASTTypes.h>
#include <vanadium/core/Program.h>
#include <vanadium/lib/FunctionRef.h>

namespace vanadium::compiler {
void FormatError(std::string& buf, const core::SourceFile& sf, std::string_view path, const ast::Range& range,
                 std::string_view message);
std::size_t PrintErrors(std::string& buf, const core::SourceFile& sf, std::string_view path,
                        lib::Consumer<const std::string&> flush);
}  // namespace vanadium::compiler
