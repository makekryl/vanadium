add_executable(vanadiumd
  src/Vanadiumd.cpp
)

target_link_libraries(vanadiumd PRIVATE stdc++exp)

target_include_directories(vanadiumd PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadiumd PRIVATE
  vanadium_lib_lserver
  vanadium_ls
)
