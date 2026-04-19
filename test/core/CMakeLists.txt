add_executable(vanadium_e2e_core
  CoreTestsRunner.cpp
)

target_link_libraries(vanadium_e2e_core PRIVATE
  GTest::gtest
  vanadium_e2e_runner
  vanadium_lib_testing

  vanadium_core

  magic_enum
)

register_e2e_test_target(vanadium_e2e_core core)
