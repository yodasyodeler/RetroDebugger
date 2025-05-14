cmake_minimum_required(VERSION 3.23)

# Only set the cxx_standard if it is not set by someone else
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example
# when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the project name and language
project(
    RetroDebugger
    VERSION 0.1.0
    DESCRIPTION "Generic library that mimics a gdb lite debug interface for retro emulators."
    LANGUAGES CXX C)

# Add .cmake files to module path. So .cmake files can be accessible in different directories.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Check if we are doing an in source build
include(NoInSourceBuilds)

# Get CPM and make it available to handle dependencies
include(cmake/get_cpm.cmake)

# TODO: The INTERFACE library NAMESPACE ALIAS are missing! See original CMake template for more info (https://github.com/cpp-best-practices/gui_starter_template)
add_library(RetroDebugger_options INTERFACE IMPORTED)
add_library(RetroDebugger_warnings INTERFACE IMPORTED)

target_compile_features(RetroDebugger_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

# Collect binaries under a stage subdirectory. Makes dependencies on Windows simpler.
# set(stageDir ${CMAKE_CURRENT_BINARY_DIR}/stage)
# include(GNUInstallDirs)
# if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
#     set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${stageDir}/${CMAKE_INSTALL_BINDIR})
# endif()
# if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
#     set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${stageDir}/${CMAKE_INSTALL_BINDIR})
# endif()
# if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
#     set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${stageDir}/${CMAKE_INSTALL_BINDIR})
# endif()

include(dependencies/fmt)
include(dependencies/NamedType)
include(dependencies/tinyxml2)
include(dependencies/magic_enum)

# Adding the src:
add_subdirectory(src bin)

# Don't even look at tests if we're not top level
if(NOT PROJECT_IS_TOP_LEVEL)
    return()
endif()

# Adding the tests:
include(CTest)
include(dependencies/gtest)

option(ENABLE_TESTING "Enable the tests" ${PROJECT_IS_TOP_LEVEL})
if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

option(ENABLE_FUZZING "Enable the fuzz tests" OFF)
if(ENABLE_FUZZING)
    message(AUTHOR_WARNING "Building Fuzz Tests, using fuzzing sanitizer https://www.llvm.org/docs/LibFuzzer.html")
    add_subdirectory(fuzz_test)
endif()

# If MSVC is being used, and ASAN is enabled, we need to set the debugger environment
# so that it behaves well with MSVC's debugger, and we can run the target from visual studio
# if(MSVC)
#     get_all_installable_targets(all_targets)
#     message("all_targets=${all_targets}")
#     set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
# endif()

# set the startup project for the "play" button in MSVC
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT RetroDebugger)

if(CMAKE_SKIP_INSTALL_RULES)
    return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
    return()
endif()

include(Install)

# Add other targets that you want installed here, be default we just package the one executable
# we know we want to ship
# package_project(
#     TARGETS
#     RetroDebugger
#     RetroDebugger_options
#     RetroDebugger_warnings
#     PUBLIC_INCLUDES
#     "src/include"
#     # PRIVATE_DEPENDENCIES_CONFIGURED RetroDebugger_options RetroDebugger_warnings
# )

# Experience shows that explicit package naming can help make it easier to sort
# out potential ABI related issues before they start, while helping you
# track a build to a specific GIT SHA
# set(CPACK_PACKAGE_FILE_NAME
#     "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
# )

# Enable folders for IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS YES)
