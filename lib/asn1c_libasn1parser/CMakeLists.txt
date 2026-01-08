add_library(asn1c_libasn1parser STATIC
  src/common/asn1_ref.c
  src/common/genhash.c

  src/asn1parser.c
  src/asn1p_alloc.c
  src/asn1p_y.c
  src/asn1p_l.c
  src/asn1p_module.c
  src/asn1p_oid.c
  src/asn1p_value.c
  src/asn1p_expr.c
  src/asn1p_xports.c
  src/asn1p_constr.c
  src/asn1p_param.c
  src/asn1p_class.c
  src/asn1p_integer.c
)

target_include_directories(asn1c_libasn1parser PUBLIC
  include
)

target_include_directories(asn1c_libasn1parser PRIVATE
  include/asn1c/libasn1common
  include/asn1c/libasn1parser
)

target_compile_options(asn1c_libasn1parser PRIVATE
  -w
)

# flex/bison output is pregenerated and checked in VCS just as in asn1c
