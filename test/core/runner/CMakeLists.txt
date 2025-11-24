
file(GLOB_RECURSE TEST_SOURCE_FILES CONFIGURE_DEPENDS src/tests/*.cpp)

add_executable(vanadium_e2e_core
  src/Runner.cpp
  ${TEST_SOURCE_FILES}
)

target_include_directories(vanadium_e2e_core PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_e2e_core PRIVATE
  GTest::gtest
  vanadium_lib_testing
  vanadium_core
  magic_enum
)

# add_test_executable(vanadium_e2e_core) # TODO: extract unit-testable parts into lib

add_test(
  NAME vanadium_e2e_core
  COMMAND vanadium_e2e_core
          --vanadium_testsuites_dir="${CMAKE_CURRENT_SOURCE_DIR}/../suites"
)
