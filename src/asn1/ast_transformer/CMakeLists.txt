add_library(vanadium_asn1_ast_transformer STATIC
  src/Asn1ModuleBasket.cpp
  src/AstTransformer.cpp
)

target_include_directories(vanadium_asn1_ast_transformer PUBLIC
  include
)

target_link_libraries(vanadium_asn1_ast_transformer PUBLIC
  vanadium_lib_arena
  vanadium_asn1_asn1c_ast
  vanadium_ast
  TBB::tbb
)

add_gtest_executable(vanadium_asn1_ast_transformer)
