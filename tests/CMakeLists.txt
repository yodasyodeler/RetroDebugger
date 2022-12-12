cmake_minimum_required(VERSION 3.16...3.23)

project(RetroDebugPackageTests LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
    enable_testing()

    find_package(RetroDebugger CONFIG REQUIRED) # for intro, project_options, ...

    if(NOT TARGET RetroDebugger::project_options)
        message(FATAL_ERROR "Requiered config package not found!")
        return() # be strictly paranoid for Template Janitor github action! CK
    endif()
endif()

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

# Source files: add configured_files and tests
target_include_directories(RetroDebuggerIntegrationTests PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
target_sources(
    RetroDebuggerIntegrationTests PRIVATE RetroDebuggerIntegrationTests.cpp
                                          "${CMAKE_BINARY_DIR}/configured_files/include/RetroDebugger_config.h")
