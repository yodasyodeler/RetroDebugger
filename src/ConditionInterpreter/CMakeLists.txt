# ######################################################################################################################
# Setup build options
# ######################################################################################################################
add_library(ConditionInterpreterLib STATIC)

target_include_directories(ConditionInterpreterLib PUBLIC "source")

target_link_libraries(
    ConditionInterpreterLib
    PUBLIC RetroDebuggerInterfaces
    PRIVATE RetroDebugger_options
            RetroDebugger_warnings
            magic_enum
            fmt::fmt
)

target_sources(
    ConditionInterpreterLib
    PRIVATE "Source/ConditionInterpreter.cpp"
            "source/ConditionInterpreter.h"
            "Source/Expr.cpp"
            "Source/Expr.h"
            "Source/IAstVisitor.h"
            "Source/IExpr.h"
            "Source/Interpreter.cpp"
            "Source/Interpreter.h"
            "Source/Parser.cpp"
            "Source/Parser.h"
            "Source/Report.cpp"
            "Source/Report.h"
            "Source/RuntimeError.h"
            "Source/Scanner.cpp"
            "Source/Scanner.h"
            "Source/StringVisitor.h"
            "Source/Token.cpp"
            "Source/Token.h"
            "Source/TokenType.cpp"
            "Source/TokenType.h"
            "Source/Types.h"
)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
