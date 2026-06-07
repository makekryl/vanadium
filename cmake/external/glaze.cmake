set(GLAZE_SOURCE_PATCH_COMMAND git apply
  # https://github.com/stephenberry/glaze/pull/2101
  ${CMAKE_CURRENT_LIST_DIR}/patches/glaze-001-fix-clang19-variant-build.patch
)

FetchContent_Declare(
  glaze          # 7.7.1
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG        ae87b187e2264ad452777bca68e35595406e9dca
  GIT_SHALLOW 1

  PATCH_COMMAND ${GLAZE_SOURCE_PATCH_COMMAND}
)

FetchContent_MakeAvailable(glaze)
