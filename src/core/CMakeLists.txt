add_library(vanadium_core STATIC
  src/ASTInspector.cpp
  src/utils/ASTUtils.cpp
  src/Parser.cpp
  src/Scanner.cpp
  src/Builtins.cpp
  src/Binder.cpp
  src/TypeChecker.cpp
  src/Program.cpp
)

target_include_directories(vanadium_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_core PUBLIC
  vanadium_lib_common
  vanadium_lib_arena
  TBB::tbb
)
target_link_libraries(vanadium_core PRIVATE
  magic_enum
)

add_test_executable(vanadium_core)
target_link_libraries(vanadium_core_test PUBLIC
  magic_enum
)


set(VANADIUM_CORE_ASTGEN_EXEC "${CMAKE_CURRENT_SOURCE_DIR}/tools/nodegen.py")
set(VANADIUM_CORE_ASTGEN_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/nodes.yml")
set(VANADIUM_CORE_ASTGEN_DEST "${CMAKE_CURRENT_SOURCE_DIR}/include/gen")
add_custom_command(
  OUTPUT ${VANADIUM_CORE_ASTGEN_DEST}
  COMMAND ${Python3_EXECUTABLE} ${VANADIUM_CORE_ASTGEN_EXEC}
                               ${VANADIUM_CORE_ASTGEN_SRC} ${VANADIUM_CORE_ASTGEN_DEST}
  DEPENDS ${VANADIUM_CORE_ASTGEN_SRC} ${VANADIUM_CORE_ASTGEN_EXEC}
  VERBATIM
  CODEGEN
  COMMENT "Generating AST Nodes"
)
add_custom_target(
  generate_vanadium_ast ALL
  DEPENDS ${VANADIUM_CORE_ASTGEN_DEST}
)

add_dependencies(vanadium_core
  generate_vanadium_ast
)
