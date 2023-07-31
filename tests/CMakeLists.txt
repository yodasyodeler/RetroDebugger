cmake_minimum_required(VERSION 3.23)

project(RetroDebugPackageTests LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
    enable_testing()
    include(CTest)
    include(GoogleTest)

    if(NOT DEFINED CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD 20)
    endif()

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
    include(Testing)

    find_package(RetroDebugger CONFIG REQUIRED) # for intro, project_options, ...

    message("Test directory")
    add_library(project_options INTERFACE IMPORTED)
    add_library(project_warnings INTERFACE IMPORTED)
    target_compile_features(project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
    # if(NOT TARGET project_options)
    #     message(FATAL_ERROR "Required config package not found!")
    #     return() # be strictly paranoid for Template Janitor github action! CK
    # endif()
endif()

add_executable(RetroDebuggerIntegrationTests)

if(PROJECT_IS_TOP_LEVEL)
    set(RetroDebugger_Lib RetroDeb::RetroDebugger)
else()
    set(RetroDebugger_Lib RetroDebugger)
endif()

target_link_libraries(
    RetroDebuggerIntegrationTests
    PUBLIC ${RetroDebugger_Lib}
    PRIVATE ${project_options}
            GTest::gtest
            GTest::gtest_main
            GTest::gmock
            GTest::gmock_main)
set_target_properties(RetroDebuggerIntegrationTests PROPERTIES FOLDER "Integration Tests")

# automatically discover tests that are defined test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
gtest_discover_tests(RetroDebuggerIntegrationTests)

# Source files: add configured_files and tests
target_include_directories(RetroDebuggerIntegrationTests PRIVATE "${CMAKE_BINARY_DIR}/bin/BuildInfo"
                                                                 "${CMAKE_BINARY_DIR}/configured_files/include")
target_sources(RetroDebuggerIntegrationTests PRIVATE RetroDebuggerIntegrationTests.cpp)
