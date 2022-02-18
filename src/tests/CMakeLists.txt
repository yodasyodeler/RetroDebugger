find_package(GTest REQUIRED)

include(CTest)
include(GoogleTest)

target_link_libraries(RetroDebuggerTests PUBLIC GTest::gtest GTest::gtest_main)
set_target_properties(RetroDebuggerTests PROPERTIES FOLDER "Unit Tests")
# target_link_libraries(catch_main PRIVATE project_options)

# Provide a simple smoke test to make sure that the CLI works and can display a --help message
add_test(NAME cli.has_help COMMAND RetroDebugger --help)

# add_executable(tests tests.cpp)
# target_link_libraries(tests PRIVATE project_warnings project_options catch_main)

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
gtest_discover_tests(
    RetroDebuggerTests
    TEST_PREFIX
        "unittests."
        REPORTER
        xml
        OUTPUT_DIR
        .
        OUTPUT_PREFIX
        "unittests."
        OUTPUT_SUFFIX
        .xml)

target_sources(RetroDebuggerTests PRIVATE BreakpointManagerTests.cpp)
