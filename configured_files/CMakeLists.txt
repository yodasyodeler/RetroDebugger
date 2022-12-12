# Create a config header with CMake project info and git sha info
configure_file("Config.h.in" "${CMAKE_BINARY_DIR}/configured_files/include/Config.h" ESCAPE_QUOTES)
