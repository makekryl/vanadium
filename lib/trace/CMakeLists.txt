add_library(vanadium_lib_trace STATIC
  src/Tracer.cpp
  src/GlobalTracer.cpp
)

target_include_directories(vanadium_lib_trace PUBLIC
  include
)

target_link_libraries(vanadium_lib_trace PUBLIC
  vanadium_lib_common
  vanadium_lib_concurrency
)

add_gtest_executable(vanadium_lib_trace)
