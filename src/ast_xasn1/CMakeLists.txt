add_library(vanadium_ast_xasn1 STATIC
  src/Asn1Scanner.cpp
  src/Asn1Transparser.cpp
)

target_include_directories(vanadium_ast_xasn1 PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_ast_xasn1 PUBLIC
  vanadium_ast
)

target_link_libraries(vanadium_ast_xasn1 PRIVATE
  vanadium_lib_common
  vanadium_lib_arena
  magic_enum
  stdc++exp # TODO: remove
)

add_gtest_executable(vanadium_ast_xasn1)
