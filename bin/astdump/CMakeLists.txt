add_executable(vanadium_astdump
  src/VanadiumDumper.cpp
)

target_include_directories(vanadium_astdump PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_astdump PRIVATE
  vanadium_core
)
