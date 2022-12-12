find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(docopt CONFIG)
find_package(tinyxml2 CONFIG)

add_library(RetroDebugger SHARED)

# Link dependencies: Wrapper method from project_options(https://github.com/cpp-best-practices/project_options)
target_link_system_libraries(
    RetroDebugger
    PRIVATE
    docopt::docopt
    fmt::fmt
    spdlog::spdlog
    tinyxml2::tinyxml2)

# project_options: includes 'NCREMENTAL:NO', from what I can tell it is a bug also present in the CMake template this is using.
# TODO: figure out impact of not including 'project_options' or the bug of including project_options.
# 'NCREMENTAL:NO' appears to be failed attempt of adding '/INCREMENTAL:NO'. -I is header include in gcc.
target_link_libraries(RetroDebugger PRIVATE project_warnings) # project_options
#target_include_directories(RetroDebugger PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include") # TODO: Uncomment when ready

# Unit Tests
if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

# Add Source files and assets
add_subdirectory(assets) # Private assets used by unit tests
add_subdirectory(include) # Files shared with RetroDebugger and binaries using this library
add_subdirectory(source) # Files private to RetroDebugger and unit tests
