FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        6910c9d9165801d8827d628cb72eb7ea9dd538c5
  GIT_SHALLOW 1
)
if(MSVC)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
