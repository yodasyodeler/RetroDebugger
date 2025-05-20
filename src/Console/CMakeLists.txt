add_library(ConsoleLib STATIC)

target_sources(
    ConsoleLib
    PRIVATE pch.h
            source/ConsoleInterpreter.h
            source/ConsoleInterpreter.cpp
            source/DebuggerPrintFormat.cpp
            source/DebuggerPrintFormat.h
            source/DebuggerStringParser.cpp
            source/DebuggerStringParser.h
)

target_include_directories(ConsoleLib PUBLIC "source")

target_link_libraries(
    ConsoleLib
    PRIVATE DebuggerLib
            RetroDebuggerInterfaces
            RetroDebugger_warnings
            RetroDebugger_options
            fmt::fmt
            NamedType
)
target_precompile_headers(ConsoleLib PRIVATE pch.h)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
