add_executable(vanadiumd
  src/Vanadiumd.cpp
)

target_include_directories(vanadiumd PRIVATE
  include
)

target_link_libraries(vanadiumd PRIVATE
  vanadium_bin_boostrap
  vanadium_bin_version
  vanadium_lib_lserver
  vanadium_ls
  argparse::argparse
)
