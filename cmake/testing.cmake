set(VANADIUM_UNIT_TEST_TARGETS "" CACHE INTERNAL "All unit test targets")
set(VANADIUM_E2E_TEST_TARGETS "" CACHE INTERNAL "All e2e test targets")

function(register_e2e_test_target test_target_name)
  gtest_discover_tests(${test_target_name}
    DISCOVERY_MODE PRE_TEST
    PROPERTIES LABELS "e2e"
  )

  list(APPEND VANADIUM_E2E_TEST_TARGETS ${test_target_name})
  set(VANADIUM_E2E_TEST_TARGETS "${VANADIUM_E2E_TEST_TARGETS}" CACHE INTERNAL "All e2e test targets")
endfunction()

function(add_gtest_executable target_name)
  set(TEST_TARGET_NAME "${target_name}_test")
  set(TEST_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/test")
  file(GLOB_RECURSE TEST_SOURCES "${TEST_SRC_DIR}/*.cpp")

  if(NOT TEST_SOURCES)
    message(AUTHOR_WARNING "No tests found for ${target_name}")
    return()
  endif()

  add_executable(${TEST_TARGET_NAME} ${TEST_SOURCES})

  target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    ${target_name}
    GTest::gtest_main
    GTest::gmock_main
  )

  gtest_discover_tests(${TEST_TARGET_NAME}
    PROPERTIES LABELS "unit"
  )

  list(APPEND VANADIUM_UNIT_TEST_TARGETS ${TEST_TARGET_NAME})
  set(VANADIUM_UNIT_TEST_TARGETS "${VANADIUM_UNIT_TEST_TARGETS}" CACHE INTERNAL "All unit test targets")
endfunction()

function(add_tests_build_target)
  add_custom_target(build_unit_tests)
  if (VANADIUM_UNIT_TEST_TARGETS)
    add_dependencies(build_unit_tests ${VANADIUM_UNIT_TEST_TARGETS})
  else()
    message(WARNING "Unit tests are not build")
  endif()

  add_custom_target(build_e2e_tests)
  if (VANADIUM_E2E_TEST_TARGETS)
    add_dependencies(build_e2e_tests ${VANADIUM_E2E_TEST_TARGETS})
  else()
    message(WARNING "E2E tests are not build")
  endif()
endfunction()
