add_library(RetroDebuggerInternal STATIC)

include(BuildInfo)
create_build_info_header(RetroDebuggerInternal)

target_sources(RetroDebuggerInternal PRIVATE pch.h include/RetroDebugger.h source/RetroDebugger.cpp)

target_include_directories(RetroDebuggerInternal PUBLIC include)

target_link_libraries(
    RetroDebuggerInternal
    PUBLIC RetroDebuggerCallbacks
           RetroDebuggerCommon
           RetroDebuggerInternalCommon
           RetroDebuggerDebugger
           RetroDebuggerStringInterpreter
           RetroDebuggerXmlParser
           RetroDebugger_warnings
           RetroDebugger_options
           fmt::fmt)

target_precompile_headers(RetroDebuggerInternal PRIVATE pch.h)

set_target_properties(RetroDebuggerInternal PROPERTIES FOLDER "RetroDebugger Code")

add_subdirectory(Common)
add_subdirectory(Debugger)
add_subdirectory(DebuggerCallbacks)
add_subdirectory(StringInterpreter)
add_subdirectory(XmlParser)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
