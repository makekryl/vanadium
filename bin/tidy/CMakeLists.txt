add_executable(vanadium_tidy
  src/VanadiumLinter.cpp
)

target_include_directories(vanadium_tidy PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_tidy PRIVATE
  vanadium_lint
  vanadium_project
  fmt::fmt
  CLI11::CLI11
)
