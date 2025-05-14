# ######################################################################################################################
# Setup build options
# ######################################################################################################################
add_library(ConditionInterpreter STATIC)

target_include_directories(
    ConditionInterpreter
    PUBLIC "Include" "Source")

target_link_libraries(
    ConditionInterpreter
    PRIVATE RetroDebugger_options
            RetroDebugger_warnings
            RetroDebuggerCommon
            magic_enum
            fmt::fmt)

target_sources(
    ConditionInterpreter
    PRIVATE "Include/ConditionInterpreter.h"
            "Source/ConditionInterpreter.cpp"
            "Source/Interpreter/Interpreter.cpp"
            "Source/Interpreter/Interpreter.h"
            "Source/Types.h"
            "Source/Parser/Parser.cpp"
            "Source/Parser/Parser.h"
            "Source/Report.cpp"
            "Source/Report.h"
            "Source/RuntimeError.h"
            "Source/Scanner/Expr.h"
            "Source/Scanner/Expr.cpp"
            "Source/Scanner/IExpr.h"
            "Source/Scanner/IAstVisitor.h"
            "Source/Scanner/Scanner.cpp"
            "Source/Scanner/Scanner.h"
            "Source/Scanner/StringVisitor.h"
            "Source/Token/Token.cpp"
            "Source/Token/Token.h"
            "Source/Token/TokenType.cpp"
            "Source/Token/TokenType.h")

target_include_directories(ConditionInterpreter PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_BINARY_DIR}/include)

if(ENABLE_TESTING)
    add_subdirectory(Tests)
endif()
