add_library(vanadium_lib_lsprotocol INTERFACE)

target_include_directories(vanadium_lib_lsprotocol INTERFACE
  include
)

target_link_libraries(vanadium_lib_lsprotocol INTERFACE
  glaze::glaze
)

add_gtest_executable(vanadium_lib_lsprotocol)
