include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG a0b8a92 # 10.0.0 release
)
FetchContent_MakeAvailable(fmt)
set_target_properties(fmt PROPERTIES FOLDER "Dependencies")
