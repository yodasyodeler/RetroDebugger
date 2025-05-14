add_library(XmlParserLib STATIC)

target_sources(
    XmlParserLib
    PRIVATE pch.h
            source/DebuggerXmlParser.h
            source/XmlElementParser.h
            source/XmlParserException.h
            source/DebuggerXmlParser.cpp
            source/XmlElementParser.cpp
            source/XmlParserException.cpp
)

target_include_directories(XmlParserLib PUBLIC source)

target_link_libraries(
    XmlParserLib
    PUBLIC RetroDebuggerInterfaces
           DebuggerInterfaces
           RetroDebugger_warnings
           RetroDebugger_options
           fmt::fmt
           tinyxml2::tinyxml2
)
target_precompile_headers(XmlParserLib PRIVATE pch.h)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
