add_executable(vanadium_fmt
  src/VanadiumFormatter.cpp
)

target_include_directories(vanadium_fmt PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_fmt PRIVATE
  vanadium_ast
)
