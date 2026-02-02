set(GLAZE_SOURCE_PATCH_COMMAND git apply
  # https://github.com/stephenberry/glaze/pull/2101
  ${CMAKE_CURRENT_LIST_DIR}/patches/glaze-001-fix-clang19-variant-build.patch
)

FetchContent_Declare(
  glaze          # 7.0.2
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG        459946d325c497c274fa59d666bd7cb6e2dd7ad0
  GIT_SHALLOW 1

  PATCH_COMMAND ${GLAZE_SOURCE_PATCH_COMMAND}
)

FetchContent_MakeAvailable(glaze)
