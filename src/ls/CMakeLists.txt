file(GLOB_RECURSE DETAIL_SOURCE_FILES CONFIGURE_DEPENDS src/detail/*.cpp)
file(GLOB_RECURSE METHODS_SOURCE_FILES CONFIGURE_DEPENDS src/methods/*.cpp)

add_library(vanadium_ls STATIC
  src/LanguageServer.cpp
  src/LanguageServerSolution.cpp
  src/LanguageServerClientMessaging.cpp
  ${DETAIL_SOURCE_FILES}
  ${METHODS_SOURCE_FILES}
)

target_include_directories(vanadium_ls PUBLIC
  interface
)
target_include_directories(vanadium_ls PRIVATE
  include
)

target_link_libraries(vanadium_ls PRIVATE
  vanadium_lib_common
  vanadium_lib_lsprotocol
  vanadium_lib_lserver
  vanadium_lib_jsonrpc
  vanadium_core
  vanadium_tooling
  vanadium_lint
  glaze::glaze
  magic_enum
)

add_gtest_executable(vanadium_ls)
