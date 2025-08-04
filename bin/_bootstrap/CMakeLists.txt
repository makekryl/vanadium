add_library(vanadium_bin_boostrap STATIC
  src/Bootstrap.cpp
)

target_include_directories(vanadium_bin_boostrap PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_bin_boostrap PRIVATE
  stdc++exp
)
