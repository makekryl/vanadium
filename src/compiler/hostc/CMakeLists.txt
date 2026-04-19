add_library(vanadium_hostc STATIC
  src/HostController.cpp
  src/RuntimeInternals.cpp
)

target_include_directories(vanadium_hostc PRIVATE
  include
)

target_link_libraries(vanadium_hostc PRIVATE
  vanadium_rt
  magic_enum
)

add_gtest_executable(vanadium_hostc)
