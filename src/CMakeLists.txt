find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(docopt CONFIG)

# Generic test that uses conan libs
add_executable(RetroDebugger main.cpp)
target_link_libraries(
    RetroDebugger
    PRIVATE project_options
            project_warnings
            docopt::docopt
            fmt::fmt
            spdlog::spdlog)
