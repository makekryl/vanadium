add_library(vanadium_lib_common INTERFACE)

target_include_directories(vanadium_lib_common INTERFACE
  include
)

add_gtest_executable(vanadium_lib_common)
