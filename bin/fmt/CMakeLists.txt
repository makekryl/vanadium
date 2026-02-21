add_executable(vanadium_fmt
  src/VanadiumFormatter.cpp
  src/OptionsReader.cpp
  src/FmtTreeDumper.cpp
)

target_include_directories(vanadium_fmt PRIVATE
  include
)

target_link_libraries(vanadium_fmt PRIVATE
  vanadium_bin_boostrap
  vanadium_format
  argparse::argparse
  reflectcpp
  fmt::fmt
)
