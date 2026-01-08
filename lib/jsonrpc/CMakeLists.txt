add_library(vanadium_lib_jsonrpc INTERFACE)

target_include_directories(vanadium_lib_jsonrpc INTERFACE
  include
)

target_link_libraries(vanadium_lib_jsonrpc INTERFACE
  glaze::glaze
)

add_gtest_executable(vanadium_lib_jsonrpc)
