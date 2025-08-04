cmake_minimum_required(VERSION 3.31)
project(vanadium)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(VANADIUM_USE_ASAN ON)

# set(VANADIUM_STATIC_BUILD ON)

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

