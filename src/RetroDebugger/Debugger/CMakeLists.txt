add_library(RetroDebuggerDebugger STATIC)

target_sources(
    RetroDebuggerDebugger
    PRIVATE pch.h
            include/BreakpointManager.h
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
           RetroDebugger_warnings
           RetroDebugger_options
           fmt::fmt
           NamedType)
set_target_properties(RetroDebuggerDebugger PROPERTIES FOLDER "RetroDebugger Code")
target_precompile_headers(RetroDebuggerDebugger PRIVATE pch.h)
