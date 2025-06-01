add_executable(vanadium_dump
  src/VanadiumDumper.cpp
)

target_include_directories(vanadium_dump PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vanadium_dump PRIVATE
  vanadium_core
)
