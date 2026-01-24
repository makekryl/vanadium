set(GENERATED_PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}/defines")
#
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
#
BISON_TARGET(
  asn1c_libasn1parser_parser
  src/asn1p_y.y
  ${GENERATED_PARSER_DIR}/asn1p_y.c
  DEFINES_FILE ${GENERATED_PARSER_DIR}/asn1p_y.h
)
FLEX_TARGET(
  asn1c_libasn1parser_lexer
  src/asn1p_l.l
  ${GENERATED_PARSER_DIR}/asn1p_l.c
  DEFINES_FILE ${GENERATED_PARSER_DIR}/asn1p_l.h
)
ADD_FLEX_BISON_DEPENDENCY(
  asn1c_libasn1parser_lexer
  asn1c_libasn1parser_parser
)

add_library(asn1c_libasn1parser STATIC
  src/common/asn1_ref.c
  src/common/genhash.cpp

  src/asn1parser.c
  src/asn1p_alloc.c
  src/asn1p_module.c
  src/asn1p_oid.c
  src/asn1p_value.c
  src/asn1p_expr.c
  src/asn1p_xports.c
  src/asn1p_constr.c
  src/asn1p_param.c
  src/asn1p_class.c
  src/asn1p_integer.c

  ${BISON_asn1c_libasn1parser_parser_OUTPUTS}
  ${FLEX_asn1c_libasn1parser_lexer_OUTPUTS}
)

target_include_directories(asn1c_libasn1parser PUBLIC
  include
)

target_include_directories(asn1c_libasn1parser PRIVATE
  include/asn1c/libasn1common
  include/asn1c/libasn1parser
  ${GENERATED_PARSER_DIR}
)

target_compile_options(asn1c_libasn1parser PRIVATE
  -w
)
