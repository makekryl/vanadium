add_library(vanadium_format STATIC
  src/VanadiumFormatter.cpp
)

target_include_directories(vanadium_format PUBLIC
  include
)

# lib -- private
target_link_libraries(vanadium_format PUBLIC
  vanadium_ast
)

add_gtest_executable(vanadium_format)
