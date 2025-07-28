add_compile_options(-Wall)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-Wextra)
endif()

if(VANADIUM_STATIC_BUILD)
  add_link_options(-static-libgcc -static-libstdc++ -static)
endif()
