find_package(fmt CONFIG)

add_executable(RetroDebuggerIntegrationTests main.cpp)
target_link_libraries(RetroDebuggerIntegrationTests PRIVATE RetroDebugger fmt::fmt)
