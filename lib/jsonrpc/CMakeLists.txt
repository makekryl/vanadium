add_library(vanadium_lib_jsonrpc INTERFACE)

target_include_directories(vanadium_lib_jsonrpc INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_lib_jsonrpc INTERFACE
  glaze::glaze
)

add_gtest_executable(vanadium_lib_jsonrpc)
