set(TBB_SOURCE_PATCH_COMMAND git apply
  ${CMAKE_CURRENT_LIST_DIR}/patches/tbb-001_fix_unused_variable_when_assertions_disabled.patch
  ${CMAKE_CURRENT_LIST_DIR}/patches/tbb-002_disable_tbbbind_build.patch
)

FetchContent_Declare(
  tbb            # 2022.1.0
  GIT_REPOSITORY https://github.com/uxlfoundation/oneTBB.git
  GIT_TAG        45587e94dfb6dfe00220c5f520020a5bc745e92f
  GIT_SHALLOW 1

  PATCH_COMMAND ${TBB_SOURCE_PATCH_COMMAND}
)

set(TBB_TEST OFF)
set(TBB_DISABLE_HWLOC_AUTOMATIC_SEARCH ON)
#
if(VANADIUM_STATIC_BUILD)
  set(BUILD_SHARED_LIBS OFF)
endif()

FetchContent_MakeAvailable(tbb)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # TODO: prevent leaking of -W to dependencies
  target_compile_options(tbb PRIVATE
    -Wno-error=stringop-overflow
  )
endif()

if(VANADIUM_USE_ASAN)
  # https://github.com/uxlfoundation/oneTBB/issues/1726
  message(NOTICE "TBB assertions are incompatible with ASAN")
  target_compile_definitions(tbb PUBLIC
    TBB_USE_ASSERT=0
  )
endif()
