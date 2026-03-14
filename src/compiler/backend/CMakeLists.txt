find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_library(vanadium_compiler STATIC
  src/Compiler.cpp
  src/CodegenContext.cpp
  src/TypeCodegen.cpp
  src/FunctionCodegen.cpp
  src/RuntimeBindings.cpp
  src/ModuleRegistrar.cpp
  src/NameProvider.cpp
)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
target_compile_definitions(vanadium_compiler PRIVATE
  ${LLVM_DEFINITIONS_LIST}
)

target_include_directories(vanadium_compiler PUBLIC
  interface
)
target_include_directories(vanadium_compiler PRIVATE
  include
  ${LLVM_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(llvm_libs
  support
  core
  irreader
  TransformUtils
)
target_link_libraries(vanadium_compiler PRIVATE
  vanadium_lib_common
  vanadium_core

  ${llvm_libs}
  magic_enum
)

add_gtest_executable(vanadium_compiler)


# TODO: this is needed to build only compiler itself with sanitizers, remove
  target_compile_options(vanadium_compiler PRIVATE "-fsanitize=${name}")
  target_link_options(vanadium_compiler PRIVATE "-fsanitize=${name}")
