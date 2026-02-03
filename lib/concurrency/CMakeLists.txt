add_library(vanadium_lib_concurrency STATIC
  src/TaskArena.cpp
  src/ThreadPool.cpp
)

target_include_directories(vanadium_lib_concurrency PUBLIC
  include
)

target_link_libraries(vanadium_lib_concurrency PUBLIC
  vanadium_lib_common
)

add_gtest_executable(vanadium_lib_concurrency)
