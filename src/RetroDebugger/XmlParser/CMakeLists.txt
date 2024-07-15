add_library(RetroDebuggerXmlParser STATIC)

target_sources(
    RetroDebuggerXmlParser
    PRIVATE pch.h
            include/DebuggerXmlParser.h
            include/XmlElementParser.h
            include/XmlParserException.h
            source/DebuggerXmlParser.cpp
            source/XmlElementParser.cpp
            source/XmlParserException.cpp)

target_include_directories(RetroDebuggerXmlParser PUBLIC include)

target_link_libraries(
    RetroDebuggerXmlParser
    PUBLIC RetroDebuggerCommon
           RetroDebuggerInternalCommon
           RetroDebugger_warnings
           RetroDebugger_options
           fmt::fmt
           tinyxml2::tinyxml2)
set_target_properties(RetroDebuggerXmlParser PROPERTIES FOLDER "RetroDebugger Code")
target_precompile_headers(RetroDebuggerXmlParser PRIVATE pch.h)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
