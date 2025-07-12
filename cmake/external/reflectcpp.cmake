FetchContent_Declare(
  reflectcpp
  GIT_REPOSITORY https://github.com/getml/reflect-cpp.git
  GIT_TAG        a70b4ecc7a3742594ff01beb43322d2f7ddfea20
  GIT_SHALLOW 1
)

set(REFLECTCPP_TOML ON)

FetchContent_MakeAvailable(reflectcpp)
