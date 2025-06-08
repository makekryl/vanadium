FetchContent_Declare(
  tbb            # 2022.1.0
  GIT_REPOSITORY https://github.com/uxlfoundation/oneTBB.git
  GIT_TAG        45587e94dfb6dfe00220c5f520020a5bc745e92f
  GIT_SHALLOW 1
)

set(TBB_TEST OFF)

FetchContent_MakeAvailable(tbb)
