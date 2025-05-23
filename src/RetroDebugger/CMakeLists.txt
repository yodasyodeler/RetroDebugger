add_library(RetroDebugger SHARED)

target_link_libraries(
    RetroDebugger
    PUBLIC RetroDebuggerInterfaces
    PRIVATE 
        RetroDebugger_options
        RetroDebugger_warnings
        ConsoleLib
        ConditionInterpreterLib
        DebuggerLib
)

include(GenerateExportHeader)
generate_export_header(
    RetroDebugger
    BASE_NAME
    RDB
    EXPORT_FILE_NAME
    "${PROJECT_NAME}_export.h"
)

# Sets up FILE_SET for RetroDebugger
# cmake-format: off
target_sources(RetroDebugger 
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}
            ${CMAKE_BINARY_DIR}/configured_files/include/
        FILES
            include/RetroDebuggerApi.h
            ${CMAKE_CURRENT_BINARY_DIR}/RetroDebugger_export.h
            ${CMAKE_BINARY_DIR}/configured_files/include/RetroDebuggerTests_assets.h)
# cmake-format: on
target_sources(RetroDebugger PRIVATE RetroDebuggerApi.cpp)
