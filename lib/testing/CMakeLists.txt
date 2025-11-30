add_library(vanadium_lib_testing STATIC
  src/gtest_helpers.cpp
)

target_include_directories(vanadium_lib_testing PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_lib_testing PUBLIC
  GTest::gtest
)

add_gtest_executable(vanadium_lib_testing)
