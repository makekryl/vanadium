add_library(vanadium_format STATIC
  src/AstPrinter.cpp
  src/AstSerializer.cpp
)

target_include_directories(vanadium_format PUBLIC
  interface
)
target_include_directories(vanadium_format PRIVATE
  include
)

target_link_libraries(vanadium_format PUBLIC
  vanadium_ast
  vanadium_lib_arena
  magic_enum # todo: remove
)

add_gtest_executable(vanadium_format)
