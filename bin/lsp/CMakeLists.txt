add_executable(vanadiumd
  src/Vanadiumd.cpp
)

target_include_directories(vanadiumd PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadiumd PRIVATE
  vanadium_core
  vanadium_lib_lsprotocol
  vanadium_lib_lsserver
)
