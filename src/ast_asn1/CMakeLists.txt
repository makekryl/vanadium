add_library(vanadium_ast_asn1 STATIC
  src/Asn1Scanner.cpp
  src/Asn1Transparser.cpp
)

target_include_directories(vanadium_ast_asn1 PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_ast_asn1 PUBLIC
  vanadium_ast
)

target_link_libraries(vanadium_ast_asn1 PRIVATE
  vanadium_lib_common
  vanadium_lib_arena
)

add_test_executable(vanadium_ast_asn1)
target_link_libraries(vanadium_ast_asn1_test PUBLIC
  magic_enum
)
