set(GLAZE_SOURCE_PATCH_COMMAND git apply
  # https://github.com/stephenberry/glaze/pull/2101
  ${CMAKE_CURRENT_LIST_DIR}/patches/glaze-001-fix-clang19-variant-build.patch
)

FetchContent_Declare(
  glaze          # 7.6.0+
  # TODO: switch back to upstream (https://github.com/stephenberry/glaze/pull/2575)
  # GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_REPOSITORY https://github.com/makekryl/glaze.git
  GIT_TAG        ceec28f302e4db0d9d40058b123f4a8809eca6e4
  GIT_SHALLOW 1

  PATCH_COMMAND ${GLAZE_SOURCE_PATCH_COMMAND}
)

FetchContent_MakeAvailable(glaze)
