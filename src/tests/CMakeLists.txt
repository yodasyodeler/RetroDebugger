find_package(GTest REQUIRED)
find_package(tinyxml2 CONFIG)

include(CTest)
include(GoogleTest)

add_executable(RetroDebuggerTests)

target_link_libraries(RetroDebuggerTests PUBLIC GTest::gtest GTest::gtest_main tinyxml2::tinyxml2)
set_target_properties(RetroDebuggerTests PROPERTIES FOLDER "Unit Tests")

# Add configured_files
target_include_directories(RetroDebuggerTests PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
target_sources(RetroDebuggerTests PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include/RetroDebugger_config.h")

# automatically discover tests that are defined test files you can modify the unittests.
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

target_sources(
    RetroDebuggerTests
    PRIVATE BreakpointManagerTests.cpp
            DebuggerApiTests.cpp
            DebuggerOperationsTests.cpp
            DebuggerStringParserTests.cpp
            DebuggerXmlParserTests.cpp
            XmlElementParserTests.cpp)
