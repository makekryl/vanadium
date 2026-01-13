add_library(vanadium_core STATIC
  src/Builtins.cpp
  src/Binder.cpp
  src/TypeChecker.cpp
  src/Program.cpp

  src/utils/SemanticUtils.cpp
)

target_include_directories(vanadium_core PUBLIC
  interface
)
target_include_directories(vanadium_core PRIVATE
  include
)

target_link_libraries(vanadium_core PUBLIC
  vanadium_ast
  vanadium_asn1_ast_transformer
  vanadium_lib_common
  vanadium_lib_arena
  TBB::tbb
)
target_link_libraries(vanadium_core PRIVATE
  magic_enum
)

add_gtest_executable(vanadium_core)
target_link_libraries(vanadium_core_test PUBLIC
  magic_enum
  vanadium_lib_testing
)
