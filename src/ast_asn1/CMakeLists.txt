add_library(vanadium_ast_asn1 STATIC
  src/Scanner.cpp
)

target_include_directories(vanadium_ast_asn1 PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_ast_asn1 PRIVATE
  vanadium_ast
  vanadium_lib_common
  vanadium_lib_arena
)

add_test_executable(vanadium_ast_asn1)
