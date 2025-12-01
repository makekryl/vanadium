add_library(vanadium_lib_lserver STATIC
  src/Channel.cpp
  src/StdioTransport.cpp
)

target_include_directories(vanadium_lib_lserver PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_lib_lserver PUBLIC
  vanadium_lib_lsprotocol
  vanadium_lib_jsonrpc
  glaze::glaze
  TBB::tbb
)

add_gtest_executable(vanadium_lib_lserver)
