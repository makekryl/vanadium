add_library(vanadium_lib_arena STATIC
  src/Arena.cpp
)

target_include_directories(vanadium_lib_arena PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_gtest_executable(vanadium_lib_arena)
