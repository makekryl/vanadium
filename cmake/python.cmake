set(Python3_FIND_VIRTUALENV ONLY)
find_package(Python3 COMPONENTS Interpreter)
if(UNIX AND EXISTS "${CMAKE_SOURCE_DIR}/.venv")
  # TODO: this is a temporary workaround
  set(Python3_EXECUTABLE "${CMAKE_SOURCE_DIR}/.venv/bin/python3")
endif()
