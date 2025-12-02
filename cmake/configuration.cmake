if(VANADIUM_STATIC_BUILD)
  add_link_options(-static-libgcc -static-libstdc++ -static)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-g1)
endif()

# add_compile_options(-ftime-trace)
