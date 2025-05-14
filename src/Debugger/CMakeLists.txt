add_library(DebuggerLib STATIC)

include(BuildInfo)
create_build_info_header(DebuggerLib)

target_include_directories(DebuggerLib PUBLIC "source")

target_link_libraries(
    DebuggerLib
    PUBLIC ConsoleLib
           DebuggerInterfaces
           RetroDebuggerInterfaces
           XmlParserLib
           RetroDebugger_warnings
           RetroDebugger_options
           fmt::fmt
           NamedType
)

target_precompile_headers(DebuggerLib PRIVATE "pch.h")

target_sources(
    DebuggerLib
    PRIVATE "pch.h"
            "source/BreakpointManager.cpp"
            "source/BreakpointManager.h"
            "source/Debugger.cpp"
            "source/Debugger.h"
            "source/DebuggerCallbacks.cpp"
            "source/DebuggerCallbacks.h"
            "source/DebuggerOperations.cpp"
            "source/DebuggerOperations.h"
            "source/RetroDebugger.cpp"
            "source/RetroDebugger.h"
)

add_subdirectory(interface)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
