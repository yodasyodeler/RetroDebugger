# Create a config header with CMake project info and git sha info
configure_file("RetroDebugger_config.h.in" "${CMAKE_BINARY_DIR}/configured_files/include/RetroDebugger_config.h" ESCAPE_QUOTES)