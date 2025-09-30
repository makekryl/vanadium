if(VANADIUM_STATIC_BUILD)
  add_link_options(-static-libgcc -static-libstdc++ -static)
endif()
