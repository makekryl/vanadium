add_compile_options(
  -Wall
  -Werror=implicit-fallthrough
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-Wextra)
endif()
