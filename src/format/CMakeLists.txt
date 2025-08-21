add_library(vanadium_format STATIC
  src/VanadiumFormatter.cpp
)

target_include_directories(vanadium_format PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# lib -- private
target_link_libraries(vanadium_format PUBLIC
  vanadium_ast
)

add_test_executable(vanadium_format)
