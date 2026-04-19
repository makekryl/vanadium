add_executable(vanadium_e2e_format
  FormatTestsRunner.cpp
)

target_link_libraries(vanadium_e2e_format PRIVATE
  GTest::gtest
  vanadium_e2e_runner
  vanadium_lib_testing

  vanadium_format
)

register_e2e_test_target(vanadium_e2e_format format)
