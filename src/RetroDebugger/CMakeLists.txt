add_library(RetroDebuggerInternal STATIC)

include(BuildInfo)
create_build_info_header(RetroDebuggerInternal)

target_sources(
    RetroDebuggerInternal
    PUBLIC include/BreakpointManager.h
           include/Debugger.h
           include/DebuggerCallbacks.h
           include/DebuggerCommon.h
           include/DebuggerConsole.h
           include/DebuggerInterpreter.h
           include/DebuggerOperations.h
           include/DebuggerPrintFormat.h
           include/DebuggerStringParser.h
           include/DebuggerXmlParser.h
           include/RetroDebugger.h
           include/XmlElementParser.h
           source/BreakpointManager.cpp
           source/Debugger.cpp
           source/DebuggerCallbacks.cpp
           source/DebuggerConsole.cpp
           source/DebuggerInterpreter.cpp
           source/DebuggerOperations.cpp
           source/DebuggerPrintFormat.cpp
           source/DebuggerStringParser.cpp
           source/DebuggerXmlParser.cpp
           source/RetroDebugger.cpp
           source/XmlElementParser.cpp)
target_include_directories(RetroDebuggerInternal PUBLIC include)

target_link_libraries(
    RetroDebuggerInternal
    PUBLIC RetroDebuggerCommon
           RetroDebugger_warnings
           RetroDebugger_options
           fmt::fmt
           tinyxml2::tinyxml2
           NamedType)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
