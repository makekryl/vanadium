add_library(vanadium_asn1_ast_transformer STATIC
  src/Asn1AstTransformer.cpp
  src/Asn1ModuleBasket.cpp
  src/ClassObjectParser.cpp
)

target_include_directories(vanadium_asn1_ast_transformer PRIVATE
  include
)
target_include_directories(vanadium_asn1_ast_transformer PUBLIC
  interface
)

target_link_libraries(vanadium_asn1_ast_transformer PUBLIC
  vanadium_lib_arena
  vanadium_asn1_asn1c_astwrapper
  vanadium_ast
  magic_enum
)

add_gtest_executable(vanadium_asn1_ast_transformer)
