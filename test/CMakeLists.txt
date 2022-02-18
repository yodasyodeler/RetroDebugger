find_package(GTest REQUIRED)

include(CTest)
include(GoogleTest)

add_executable(RetroDebuggerTests tests.cpp)
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

# # Add a file containing a set of constexpr tests
# add_executable(constexpr_tests constexpr_tests.cpp)
# target_link_libraries(constexpr_tests PRIVATE project_options project_warnings catch_main)

# gtest_discover_tests(
#   constexpr_tests
#   TEST_PREFIX
#   "constexpr."
#   REPORTER
#   xml
#   OUTPUT_DIR
#   .
#   OUTPUT_PREFIX
#   "constexpr."
#   OUTPUT_SUFFIX
#   .xml)

# Disable the constexpr portion of the test, and build again this allows us to have an executable that we can debug when
# things go wrong with the constexpr testing
#add_executable(relaxed_constexpr_tests constexpr_tests.cpp)
#target_link_libraries(relaxed_constexpr_tests PRIVATE project_options project_warnings catch_main)
#target_compile_definitions(relaxed_constexpr_tests PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)

# gtest_discover_tests(
#   relaxed_constexpr_tests
#   TEST_PREFIX
#   "relaxed_constexpr."
#   REPORTER
#   xml
#   OUTPUT_DIR
#   .
#   OUTPUT_PREFIX
#   "relaxed_constexpr."
#   OUTPUT_SUFFIX
#   .xml)
