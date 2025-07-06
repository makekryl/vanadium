file(GLOB_RECURSE DOMAIN_SOURCE_FILES CONFIGURE_DEPENDS src/domain/*.cpp)
file(GLOB_RECURSE METHODS_SOURCE_FILES CONFIGURE_DEPENDS src/methods/*.cpp)

add_library(vanadium_ls STATIC
  src/LangaugeServer.cpp
  ${DOMAIN_SOURCE_FILES}
  ${METHODS_SOURCE_FILES}
)

target_include_directories(vanadium_ls PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/interface
)
target_include_directories(vanadium_ls PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_ls PRIVATE
  vanadium_lib_common
  vanadium_lib_lsprotocol
  vanadium_lib_lserver
  vanadium_core
  vanadium_project
  vanadium_lint
  glaze::glaze
  magic_enum
)

add_test_executable(vanadium_ls)
