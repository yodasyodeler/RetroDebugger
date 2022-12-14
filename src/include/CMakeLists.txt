include(GenerateExportHeader)
generate_export_header(
    RetroDebugger
    BASE_NAME
    RDB
    EXPORT_FILE_NAME
    "${PROJECT_NAME}_export.h")

target_sources(RetroDebugger PRIVATE RetroDebuggerApi.h RetroDebuggerCommon.h ${CMAKE_CURRENT_BINARY_DIR}/RetroDebugger_export.h)

source_group(include REGULAR_EXPRESSION *.h)
target_include_directories(
    RetroDebugger PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
                         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if(ENABLE_TESTING)
    target_sources(RetroDebuggerTests PRIVATE RetroDebuggerApi.h RetroDebuggerCommon.h)
    target_include_directories(RetroDebuggerTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_BINARY_DIR})
endif()
