find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(docopt CONFIG)
find_package(tinyxml2 CONFIG)

#TODO make a library
#add_library(RetroDebugger SHARED)
add_executable(RetroDebugger main.cpp)
target_link_libraries(
    RetroDebugger
    PRIVATE project_options
            project_warnings
            docopt::docopt
            fmt::fmt
            spdlog::spdlog
            tinyxml2::tinyxml2)

#TODO
#add_subdirectory(source)

# Unit Tests
if(ENABLE_RETRODEBUGGER_UNIT_TESTS)
    add_executable(RetroDebuggerTests)
    add_subdirectory(tests)
endif()
