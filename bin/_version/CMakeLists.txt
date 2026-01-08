set(VANADIUM_BIN_VERSION_OUTPUT src/version.cpp)

add_custom_command(
  OUTPUT ${VANADIUM_BIN_VERSION_OUTPUT}
  COMMAND ${CMAKE_COMMAND}
          -D WDIR=${PROJECT_SOURCE_DIR}
          -D SRC=${CMAKE_CURRENT_SOURCE_DIR}/${VANADIUM_BIN_VERSION_OUTPUT}.in
          -D DST=${CMAKE_CURRENT_BINARY_DIR}/${VANADIUM_BIN_VERSION_OUTPUT}
          -P ${PROJECT_SOURCE_DIR}/cmake/tools/git_version.cmake
  VERBATIM
  CODEGEN
  COMMENT "Extracting revision from git"
)

add_library(vanadium_bin_version STATIC
  ${VANADIUM_BIN_VERSION_OUTPUT}
)

target_include_directories(vanadium_bin_version PUBLIC
  include
)

