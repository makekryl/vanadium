add_library(vanadium_lib_common INTERFACE)

target_include_directories(vanadium_lib_common INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_gtest_executable(vanadium_lib_common)
