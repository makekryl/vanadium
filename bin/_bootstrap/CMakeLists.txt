add_library(vanadium_bin_boostrap STATIC
  src/Bootstrap.cpp
)

target_include_directories(vanadium_bin_boostrap PUBLIC
  include
)

target_link_libraries(vanadium_bin_boostrap PRIVATE
  stdc++exp
)
