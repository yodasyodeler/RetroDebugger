# Copy files
file(COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/GameboyOperationsDebugger.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/example_operations.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/invalid_empty.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/valid_bare_min.xml
    DESTINATION
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
    message("Debugger output directory: " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
