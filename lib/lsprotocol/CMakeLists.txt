add_library(vanadium_lib_lsprotocol STATIC
  src/LSProtocolSerializers.cpp
)

target_include_directories(vanadium_lib_lsprotocol PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_lib_lsprotocol PUBLIC
  vanadium_lib_arena
  ryml::ryml
)

add_test_executable(vanadium_lib_lsprotocol)
