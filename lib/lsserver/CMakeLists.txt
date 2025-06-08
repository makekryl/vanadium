add_library(vanadium_lib_lsserver STATIC
  src/Channel.cpp
  src/StdioTransport.cpp
)

target_include_directories(vanadium_lib_lsserver PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_lib_lsserver PUBLIC
  vanadium_lib_lsprotocol
  TBB::tbb
)

add_test_executable(vanadium_lib_lsserver)
