include(GenerateExportHeader)
generate_export_header(RetroDebugger)

target_sources(RetroDebugger PRIVATE # FILE_SET # HEADERS # FILES
                                     DebuggerApi.h DebuggerCommon.h ${CMAKE_CURRENT_BINARY_DIR}/retrodebugger_export.h)

source_group(include REGULAR_EXPRESSION *.h)
target_include_directories(RetroDebugger PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
                                                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
# message("Directory install:" "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
#         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if(ENABLE_TESTING)
    target_sources(RetroDebuggerTests PRIVATE DebuggerApi.h DebuggerCommon.h)
    target_include_directories(RetroDebuggerTests PRIVATE ${CMAKE_CURRENT_LIST_DIR})
endif()
