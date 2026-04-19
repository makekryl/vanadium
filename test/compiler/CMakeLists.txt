find_package(LLVM REQUIRED CONFIG QUIET)

add_executable(vanadium_e2e_compiler
  CompilerTestsRunner.cpp
)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
target_compile_definitions(vanadium_compiler PRIVATE
  ${LLVM_DEFINITIONS_LIST}
)

target_include_directories(vanadium_e2e_compiler PRIVATE
  ${LLVM_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(llvm_libs
  Core
  OrcJIT
  ExecutionEngine
  RuntimeDyld
  native
  Support
)

target_link_libraries(vanadium_e2e_compiler PRIVATE
  GTest::gtest
  vanadium_lib_testing
  vanadium_e2e_runner

  vanadium_core
  vanadium_compiler
  vanadium_rt

  magic_enum
  ${llvm_libs}
)

target_link_options(vanadium_e2e_compiler PRIVATE
  -rdynamic
  "-Wl,--whole-archive" $<TARGET_FILE:vanadium_rt> "-Wl,--no-whole-archive"
)

register_e2e_test_target(vanadium_e2e_compiler compiler)
