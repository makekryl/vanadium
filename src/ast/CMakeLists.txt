add_library(vanadium_ast STATIC
  src/ASTInspector.cpp
  src/utils/ASTUtils.cpp
  src/Parser.cpp
  src/Scanner.cpp
)

target_include_directories(vanadium_ast PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_ast PUBLIC
  vanadium_lib_common
  vanadium_lib_arena
)
target_link_libraries(vanadium_ast PRIVATE
  magic_enum
)

add_gtest_executable(vanadium_ast)


set(VANADIUM_AST_ASTGEN_EXEC "${CMAKE_CURRENT_SOURCE_DIR}/tools/nodegen.py")
set(VANADIUM_AST_ASTGEN_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/nodes.yml")
set(VANADIUM_AST_ASTGEN_DEST "${CMAKE_CURRENT_SOURCE_DIR}/include/vanadium/ast/gen")
add_custom_command(
  OUTPUT ${VANADIUM_AST_ASTGEN_DEST}
  COMMAND ${Python3_EXECUTABLE} ${VANADIUM_AST_ASTGEN_EXEC}
                                ${VANADIUM_AST_ASTGEN_SRC} ${VANADIUM_AST_ASTGEN_DEST}
  DEPENDS ${VANADIUM_AST_ASTGEN_SRC} ${VANADIUM_AST_ASTGEN_EXEC}
  VERBATIM
  CODEGEN
  COMMENT "Generating AST Nodes"
)
add_custom_target(
  generate_vanadium_ast ALL
  DEPENDS ${VANADIUM_AST_ASTGEN_DEST}
)

add_dependencies(vanadium_ast
  generate_vanadium_ast
)
