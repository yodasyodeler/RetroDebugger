add_library(RetroDebuggerXmlParser STATIC)

target_sources(
    RetroDebuggerXmlParser
    PRIVATE include/DebuggerXmlParser.h
            include/XmlElementParser.h
            source/DebuggerXmlParser.cpp
            source/XmlElementParser.cpp)

target_include_directories(RetroDebuggerXmlParser PUBLIC include)

target_link_libraries(
    RetroDebuggerXmlParser
    PUBLIC RetroDebuggerCommon
            RetroDebuggerInternalCommon
            RetroDebugger_warnings
            RetroDebugger_options
            fmt::fmt
            tinyxml2::tinyxml2)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
