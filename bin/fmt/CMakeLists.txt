add_executable(vanadium_fmt
  src/VanadiumFormatter.cpp
)

target_include_directories(vanadium_fmt PRIVATE
  include
)

target_link_libraries(vanadium_fmt PRIVATE
  vanadium_ast
)
