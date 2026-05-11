add_subdirectory(_version)
add_subdirectory(_bootstrap)

add_subdirectory(astdump)
add_subdirectory(fmt)
add_subdirectory(tidy)
add_subdirectory(lsp)
add_subdirectory(compiler)

###

set(VANADIUM_TOOLS_TARGETS
  vanadium_astdump
  vanadium_fmt
  vanadiumd
  vanadium_tidy
  vanadiumc
)

add_custom_target(vanadium_tools DEPENDS ${VANADIUM_TOOLS_TARGETS})

install(TARGETS ${VANADIUM_TOOLS_TARGETS}
  RUNTIME DESTINATION bin
    COMPONENT runtime
  LIBRARY DESTINATION lib
    COMPONENT runtime
)
