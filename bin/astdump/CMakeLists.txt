add_executable(vanadium_astdump
  src/VanadiumDumper.cpp
)

target_include_directories(vanadium_astdump PRIVATE
  include
)

target_link_libraries(vanadium_astdump PRIVATE
  vanadium_bin_boostrap
  vanadium_ast
  vanadium_asn1_ast_transformer
  argparse::argparse
)
