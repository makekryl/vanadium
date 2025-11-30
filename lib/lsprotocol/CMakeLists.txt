add_library(vanadium_lib_lsprotocol INTERFACE)

target_include_directories(vanadium_lib_lsprotocol INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_lib_lsprotocol INTERFACE
  glaze::glaze
)

add_gtest_executable(vanadium_lib_lsprotocol)
