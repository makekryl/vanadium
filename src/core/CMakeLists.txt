add_library(vanadium_core STATIC
  src/Builtins.cpp
  src/Binder.cpp
  src/TypeChecker.cpp
  src/Program.cpp
)

target_include_directories(vanadium_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_core PUBLIC
  vanadium_ast
  vanadium_lib_common
  vanadium_lib_arena
  TBB::tbb
)
target_link_libraries(vanadium_core PRIVATE
  magic_enum
)

add_test_executable(vanadium_core)
target_link_libraries(vanadium_core_test PUBLIC
  magic_enum
)
