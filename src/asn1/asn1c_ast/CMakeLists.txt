add_library(vanadium_asn1_asn1c_ast STATIC
  src/Asn1cAstWrapper.cpp
)

target_include_directories(vanadium_asn1_asn1c_ast PUBLIC
  include
)

target_link_libraries(vanadium_asn1_asn1c_ast PUBLIC
  asn1c_libasn1parser
  vanadium_lib_arena
)

add_gtest_executable(vanadium_asn1_asn1c_ast)
