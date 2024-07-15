# Set paths for Asset files
set(GameboyOperationsDebuggerXml ${CMAKE_CURRENT_SOURCE_DIR}/GameboyOperationsDebugger.xml)
set(ExampleOperationsXml ${CMAKE_CURRENT_SOURCE_DIR}/example_operations.xml)
set(InvalidEmptyXml ${CMAKE_CURRENT_SOURCE_DIR}/invalid_empty.xml)
set(ValidBaseMinXml ${CMAKE_CURRENT_SOURCE_DIR}/valid_bare_min.xml)

configure_file("RetroDebuggerTests_assets.h.in"
               "${CMAKE_BINARY_DIR}/configured_files/include/RetroDebuggerTests_assets.h" ESCAPE_QUOTES)
