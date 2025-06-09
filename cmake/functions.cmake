function(add_test_executable target_name)
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

  gtest_discover_tests(${TEST_TARGET_NAME})
endfunction()
