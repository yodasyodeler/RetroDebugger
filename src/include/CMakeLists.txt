include(GenerateExportHeader)
generate_export_header(
    RetroDebugger
    BASE_NAME
    RDB
    EXPORT_FILE_NAME
    "${PROJECT_NAME}_export.h")

#target_sources(RetroDebugger PRIVATE RetroDebuggerApi.h RetroDebuggerCommon.h ${CMAKE_CURRENT_BINARY_DIR}/include/RetroDebugger_export.h)

#source_group(include REGULAR_EXPRESSION *.h)

if(ENABLE_TESTING)
    target_sources(RetroDebuggerTests PRIVATE RetroDebuggerCommon.h)
    target_include_directories(RetroDebuggerTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_BINARY_DIR}/include)
endif()
