include(FetchContent)

function(_vanadium_external external_name)
  include(${CMAKE_CURRENT_LIST_DIR}/external/${external_name}.cmake)
endfunction()

_vanadium_external(gtest)
_vanadium_external(magic_enum)
_vanadium_external(glaze)
_vanadium_external(tbb)
_vanadium_external(fmt)
_vanadium_external(argparse)
_vanadium_external(tomlplusplus)
_vanadium_external(reflectcpp)
