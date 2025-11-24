find_package(Git)

if(GIT_EXECUTABLE)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --dirty=+
    WORKING_DIRECTORY ${WDIR}
    OUTPUT_VARIABLE VANADIUM_GIT_VERSION
    RESULT_VARIABLE ERROR_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()

if(VANADIUM_GIT_VERSION STREQUAL "")
  set(VANADIUM_GIT_VERSION unknown)
  message(WARNING "Failed to determine version from Git tags. Using default version '${VANADIUM_GIT_VERSION}'")
endif()

configure_file(${SRC} ${DST} @ONLY)
