add_library(vanadium_lib_testing STATIC
  src/gtest_helpers.cpp
)

target_include_directories(vanadium_lib_testing PUBLIC
  include
)

target_link_libraries(vanadium_lib_testing PUBLIC
  GTest::gtest
)
