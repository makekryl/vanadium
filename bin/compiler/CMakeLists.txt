add_executable(vanadiumc
  src/Vanadiumc.cpp
)

target_include_directories(vanadiumc PRIVATE
  include
)

target_link_libraries(vanadiumc PRIVATE
  vanadium_bin_boostrap
  vanadium_core
  vanadium_compiler
  argparse::argparse
)
