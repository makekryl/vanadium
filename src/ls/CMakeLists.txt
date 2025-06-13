file(GLOB_RECURSE
  PROCEDURES_SOURCE_FILES CONFIGURE_DEPENDS
  src/procedures/*.cpp
)

add_library(vanadium_ls STATIC
  src/LangaugeServer.cpp
  ${PROCEDURES_SOURCE_FILES}
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
)

add_test_executable(vanadium_ls)
