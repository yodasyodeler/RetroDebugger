# ######################################################################################################################
# Add Source
# ######################################################################################################################
include(GoogleTest)
include(CTest)

enable_testing()
add_executable(ConditionInterpreterTests)

target_include_directories(ConditionInterpreterTests PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_sources(
    ConditionInterpreterTests
    PRIVATE ExprTests.cpp
            ParserExpressionTests.cpp
            InterpreterExpressionTests.cpp
            ScannerTests.cpp)

target_link_libraries(
    ConditionInterpreterTests
    PRIVATE RetroDebugger_options
            RetroDebugger_warnings
            ConditionInterpreter
            fmt::fmt
            gtest
            gtest_main
            gmock
            gmock_main)

gtest_discover_tests(ConditionInterpreterTests)
