# ######################################################################################################################
# Add Source
# ######################################################################################################################
include(GoogleTest)
include(CTest)

enable_testing()
add_executable(ConditionInterpreterLibTests)

target_sources(
    ConditionInterpreterLibTests
    PRIVATE ConditionInterpreterTests.cpp
            ExprTests.cpp
            ParserExpressionTests.cpp
            InterpreterExpressionTests.cpp
            ScannerTests.cpp
)

target_link_libraries(
    ConditionInterpreterLibTests
    PRIVATE DebuggerLib
            RdbMocks
            RetroDebugger_options
            RetroDebugger_warnings
            ConditionInterpreterLib
            fmt::fmt
            gtest
            gtest_main
            gmock
            gmock_main
)

set_target_properties(ConditionInterpreterLibTests PROPERTIES FOLDER "Unit Tests")


gtest_discover_tests(ConditionInterpreterLibTests)
