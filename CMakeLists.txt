cmake_minimum_required(VERSION 3.31)
project(vanadium)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
# set(VANADIUM_USE_ASAN ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release") # TODO
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(configuration)
include(sanitizers)
include(python)
include(externals)
include(functions)

enable_testing()

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(bin)

