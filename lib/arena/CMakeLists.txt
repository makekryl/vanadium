add_library(vanadium_lib_arena STATIC
  src/Arena.cpp
)

target_include_directories(vanadium_lib_arena PUBLIC
  include
)

add_gtest_executable(vanadium_lib_arena)
