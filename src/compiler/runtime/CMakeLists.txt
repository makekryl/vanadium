add_library(vanadium_rt STATIC
  src/runtime.cpp
  src/rt_alloc.cpp
  src/rt_reflect.cpp
  src/rt_integer.cpp
  src/rt_float.cpp
  src/rt_boolean.cpp
  src/rt_charstring.cpp
  src/rt_bitstring.cpp
  src/rt_octetstring.cpp
  src/rt_hexstring.cpp
  src/rt_optional.cpp
  src/rt_verdict.cpp
  src/Entrypoint.cpp
)

target_include_directories(vanadium_rt PUBLIC
  interface
)
target_include_directories(vanadium_rt PRIVATE
  include
)

target_link_libraries(vanadium_rt PRIVATE
  magic_enum
)

add_gtest_executable(vanadium_rt)
