add_library(vanadium_tooling STATIC
  src/Project.cpp
  src/Solution.cpp
  src/impl/SystemFS.cpp
)

target_include_directories(vanadium_tooling PUBLIC
  include
)

target_link_libraries(vanadium_tooling PRIVATE
  vanadium_lib_common
  vanadium_lib_arena
)
target_link_libraries(vanadium_tooling PUBLIC
  vanadium_core
  reflectcpp
)

add_gtest_executable(vanadium_tooling)
