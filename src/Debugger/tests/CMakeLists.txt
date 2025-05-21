include(CTest)
include(GoogleTest)

add_executable(RetroDebuggerTests)

# Add configured_files
target_include_directories(RetroDebuggerTests PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")

target_sources(
    RetroDebuggerTests
    PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include/RetroDebuggerTests_assets.h"
            BreakpointManagerTests.cpp
            DebuggerOperationsTests.cpp
            DebuggerStringParserTests.cpp
            DebuggerXmlParserTests.cpp
            XmlElementParserTests.cpp)

target_link_libraries(
    RetroDebuggerTests
    PRIVATE DebuggerLib
            RdbMocks
            GTest::gtest
            GTest::gtest_main
            GTest::gmock
            GTest::gmock_main)
set_target_properties(RetroDebuggerTests PROPERTIES FOLDER "Unit Tests")

# automatically discover tests that are defined test files you can modify the unit tests.
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
        .xml
)