add_library(vanadium_asn1_xast STATIC
  src/Asn1Scanner.cpp
  src/Asn1Transparser.cpp
)

target_include_directories(vanadium_asn1_xast PUBLIC
  include
)

target_link_libraries(vanadium_asn1_xast PUBLIC
  vanadium_ast
)

target_link_libraries(vanadium_asn1_xast PRIVATE
  vanadium_lib_common
  vanadium_lib_arena
  magic_enum
  stdc++exp # TODO: remove
)

add_gtest_executable(vanadium_asn1_xast)
