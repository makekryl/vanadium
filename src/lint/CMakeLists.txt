file(GLOB_RECURSE BUILTIN_RULES_FILES CONFIGURE_DEPENDS src/rules/*.cpp)

add_library(vanadium_lint STATIC
  src/Linter.cpp
  ${BUILTIN_RULES_FILES}
)

target_include_directories(vanadium_lint PUBLIC
  include
)

target_link_libraries(vanadium_lint PUBLIC
  vanadium_lib_common
  vanadium_core
)

add_gtest_executable(vanadium_format)
