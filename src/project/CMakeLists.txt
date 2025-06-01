add_library(vanadium_project STATIC
  src/Project.cpp
)

target_include_directories(vanadium_project PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_project PRIVATE
  vanadium_lib_common
  vanadium_lib_arena
  ryml::ryml
)
target_link_libraries(vanadium_project PUBLIC
  vanadium_core
)

add_test_executable(vanadium_project)
