target_sources(
    RetroDebugger
    PRIVATE BreakpointManager.cpp
            BreakpointManager.h
            Debugger.cpp
            Debugger.h
            DebuggerCallbacks.cpp
            DebuggerCallbacks.h
            DebuggerCommon.h
            DebuggerConsole.cpp
            DebuggerConsole.h
            DebuggerInterpreter.cpp
            DebuggerInterpreter.h
            DebuggerOperations.cpp
            DebuggerOperations.h
            DebuggerPrintFormat.cpp
            DebuggerPrintFormat.h
            DebuggerStringParser.cpp
            DebuggerStringParser.h
            DebuggerXmlParser.cpp
            DebuggerXmlParser.h
            RetroDebuggerApi.cpp
            RetroDebugger.cpp
            RetroDebugger.h
            XmlElementParser.cpp
            XmlElementParser.h)
target_include_directories(RetroDebugger PRIVATE ${CMAKE_CURRENT_LIST_DIR})
#source_group(include REGULAR_EXPRESSION *.h)
#source_group(source REGULAR_EXPRESSION *.cpp)

if(ENABLE_TESTING)
    target_sources(
        RetroDebuggerTests
        PRIVATE BreakpointManager.cpp
                BreakpointManager.h
                Debugger.cpp
                Debugger.h
                DebuggerCallbacks.cpp
                DebuggerCallbacks.h
                DebuggerCommon.h
                DebuggerConsole.cpp
                DebuggerConsole.h
                DebuggerInterpreter.cpp
                DebuggerInterpreter.h
                DebuggerOperations.cpp
                DebuggerOperations.h
                DebuggerPrintFormat.cpp
                DebuggerPrintFormat.h
                DebuggerStringParser.cpp
                DebuggerStringParser.h
                DebuggerXmlParser.cpp
                DebuggerXmlParser.h
                XmlElementParser.cpp
                XmlElementParser.h)
    target_include_directories(RetroDebuggerTests PRIVATE ${CMAKE_CURRENT_LIST_DIR})
endif()
