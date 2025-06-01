add_library(vanadium_lint STATIC
  src/Linter.cpp
  src/PrecompiledRules.cpp
)

target_include_directories(vanadium_lint PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_lint PUBLIC
  vanadium_lib_common
)
target_link_libraries(vanadium_lint PUBLIC
  vanadium_core
)

add_test_executable(vanadium_format)
