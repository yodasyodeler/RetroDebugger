find_package(GTest REQUIRED)
find_package(fmt CONFIG)

#include(CTest)
#include(GoogleTest)

add_executable(RetroDebuggerIntegrationTests)

target_link_libraries(
    RetroDebuggerIntegrationTests
    PRIVATE RetroDebugger
            fmt::fmt
            GTest::gtest
            GTest::gtest_main)
set_target_properties(RetroDebuggerIntegrationTests PROPERTIES FOLDER "Integration Tests")

# automatically discover tests that are defined test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
gtest_discover_tests(
    RetroDebuggerIntegrationTests
    TEST_PREFIX
        "integrationtests."
        REPORTER
        xml
        OUTPUT_DIR
        .
        OUTPUT_PREFIX
        "integrationtests."
        OUTPUT_SUFFIX
        .xml)

target_sources(RetroDebuggerIntegrationTests PRIVATE RetroDebuggerIntegrationTests.cpp)
