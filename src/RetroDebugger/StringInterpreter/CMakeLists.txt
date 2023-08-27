add_library(RetroDebuggerStringInterpreter STATIC)

target_sources(
    RetroDebuggerStringInterpreter
    PRIVATE include/DebuggerConsole.h
            include/DebuggerInterpreter.h
            include/DebuggerPrintFormat.h
            include/DebuggerStringParser.h
            source/DebuggerConsole.cpp
            source/DebuggerInterpreter.cpp
            source/DebuggerPrintFormat.cpp
            source/DebuggerStringParser.cpp)

target_include_directories(RetroDebuggerStringInterpreter PUBLIC include)

target_link_libraries(
    RetroDebuggerStringInterpreter
    PRIVATE RetroDebuggerCallbacks
            RetroDebuggerCommon
            RetroDebuggerDebugger
            RetroDebuggerInternalCommon
            RetroDebugger_warnings
            RetroDebugger_options
            fmt::fmt
            NamedType)
set_target_properties(RetroDebuggerStringInterpreter PROPERTIES FOLDER "RetroDebugger Code")

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
