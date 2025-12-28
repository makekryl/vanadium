add_library(vanadium_asn1_ast STATIC
  src/Asn1Scanner.cpp
  src/Asn1Parser.cpp
)

target_include_directories(vanadium_asn1_ast PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_asn1_ast PUBLIC
  vanadium_ast
)

target_link_libraries(vanadium_asn1_ast PRIVATE
  vanadium_lib_common
  vanadium_lib_arena
  magic_enum
)

add_gtest_executable(vanadium_asn1_ast)


set(VANADIUM_ASN1_AST_ASTGEN_EXEC "${CMAKE_CURRENT_SOURCE_DIR}/tools/nodegen.py")
set(VANADIUM_ASN1_AST_ASTGEN_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/nodes.yml")
set(VANADIUM_ASN1_AST_ASTGEN_DEST "${CMAKE_CURRENT_SOURCE_DIR}/include/vanadium/asn1/ast/gen")
add_custom_command(
  OUTPUT ${VANADIUM_ASN1_AST_ASTGEN_DEST}
  COMMAND ${Python3_EXECUTABLE} ${VANADIUM_ASN1_AST_ASTGEN_EXEC}
                                ${VANADIUM_ASN1_AST_ASTGEN_SRC} ${VANADIUM_ASN1_AST_ASTGEN_DEST}
  DEPENDS ${VANADIUM_ASN1_AST_ASTGEN_SRC} ${VANADIUM_ASN1_AST_ASTGEN_EXEC}
  VERBATIM
  CODEGEN
  COMMENT "Generating ASN1 AST Nodes"
)
add_custom_target(
  generate_vanadium_asn1_ast ALL
  DEPENDS ${VANADIUM_ASN1_AST_ASTGEN_DEST}
)

add_dependencies(vanadium_asn1_ast
  generate_vanadium_asn1_ast
)
