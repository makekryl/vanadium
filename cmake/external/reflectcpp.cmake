# TODO: remove reflectcpp & tomlplusplus when glaze fully supports TOML

set(REFLECTCPP_SOURCE_PATCH_COMMAND git apply
  ${CMAKE_CURRENT_LIST_DIR}/patches/reflectcpp-001_quiet_findpackage_tomlplusplus.patch
)

FetchContent_Declare(
  reflectcpp     # 0.23.0
  GIT_REPOSITORY https://github.com/getml/reflect-cpp.git
  GIT_TAG        09e17b2b8578e95625fc1fc92ec863d2583e1723
  GIT_SHALLOW 1
  GIT_SUBMODULES ""  # skip vcpkg cloning

  PATCH_COMMAND ${REFLECTCPP_SOURCE_PATCH_COMMAND}
)

set(REFLECTCPP_JSON OFF)
set(REFLECTCPP_TOML ON)

FetchContent_MakeAvailable(reflectcpp)
