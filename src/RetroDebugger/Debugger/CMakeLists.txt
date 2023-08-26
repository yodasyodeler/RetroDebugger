add_library(RetroDebuggerDebugger STATIC)

target_sources(
    RetroDebuggerDebugger
    PRIVATE include/BreakpointManager.h
            include/Debugger.h
            include/DebuggerOperations.h
            source/BreakpointManager.cpp
            source/Debugger.cpp
            source/DebuggerOperations.cpp)

target_include_directories(RetroDebuggerDebugger PUBLIC include)

target_link_libraries(
    RetroDebuggerDebugger
    PUBLIC RetroDebuggerCallbacks
           RetroDebuggerCommon
           RetroDebuggerInternalCommon
           RetroDebuggerXmlParser
           RetroDebugger_warnings
           RetroDebugger_options
           fmt::fmt
           NamedType)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
