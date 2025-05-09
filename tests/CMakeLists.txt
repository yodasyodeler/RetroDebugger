cmake_minimum_required(VERSION 3.23)

project(RetroDebugPackageTests LANGUAGES CXX)

add_executable(RetroDebuggerIntegrationTests)

# Get CPM and make it available to handle dependencies
include(../cmake/get_cpm.cmake)

include(CTest)
include(GoogleTest)
if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
    enable_testing()

    if(NOT DEFINED CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD 20)
    endif()

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
    include(dependencies/gtest)

    find_package(RetroDebugger CONFIG REQUIRED) # for intro, RetroDebugger_options, ...

    message("Test directory")
    add_library(RetroDebugger_options INTERFACE IMPORTED)
    add_library(RetroDebugger_warnings INTERFACE IMPORTED)
    target_compile_features(RetroDebugger_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
    target_link_libraries(RetroDebuggerIntegrationTests PRIVATE RetroDeb::RetroDebugger)
else()
    target_link_libraries(RetroDebuggerIntegrationTests PUBLIC RetroDebugger)
endif()

#TODO: fFgure out if I should be looking at supporting `BUILD_SHARED_LIBS` to allow switch between static and shared
if(WIN32)
    add_custom_command(
        TARGET RetroDebuggerIntegrationTests
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:RetroDebuggerIntegrationTests>
                $<TARGET_FILE_DIR:RetroDebuggerIntegrationTests>
        COMMAND_EXPAND_LISTS)
endif()

target_link_libraries(
    RetroDebuggerIntegrationTests
    PRIVATE ${RetroDebugger_options}
            ${RetroDebugger_warnings}
            GTest::gtest
            GTest::gtest_main
            GTest::gmock
            GTest::gmock_main)
set_target_properties(RetroDebuggerIntegrationTests PROPERTIES FOLDER "Integration Tests")

# automatically discover tests that are defined test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
gtest_discover_tests(RetroDebuggerIntegrationTests)

# Source files: add configured_files and tests
target_include_directories(RetroDebuggerIntegrationTests PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
target_sources(RetroDebuggerIntegrationTests PRIVATE RetroDebuggerIntegrationTests.cpp RetroDebuggerExamples.cpp)
