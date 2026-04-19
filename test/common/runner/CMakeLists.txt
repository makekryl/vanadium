add_library(vanadium_e2e_runner STATIC
  src/Runner.cpp
)

target_include_directories(vanadium_e2e_runner PUBLIC
  include
)

target_link_libraries(vanadium_e2e_runner PRIVATE
  GTest::gtest
  vanadium_lib_testing

  vanadium_core

  magic_enum
)
