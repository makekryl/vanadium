# TODO: remove reflectcpp & tomlplusplus when glaze fully supports TOML

set(REFLECTCPP_SOURCE_PATCH_COMMAND git apply
  ${CMAKE_CURRENT_LIST_DIR}/patches/reflectcpp-001_quiet_findpackage_tomlplusplus.patch
)

FetchContent_Declare(
  reflectcpp
  GIT_REPOSITORY https://github.com/getml/reflect-cpp.git
  GIT_TAG        a70b4ecc7a3742594ff01beb43322d2f7ddfea20
  GIT_SHALLOW 1
  GIT_SUBMODULES ""  # skip vcpkg cloning

  PATCH_COMMAND ${REFLECTCPP_SOURCE_PATCH_COMMAND}
)

set(REFLECTCPP_JSON OFF)
set(REFLECTCPP_TOML ON)

FetchContent_MakeAvailable(reflectcpp)
