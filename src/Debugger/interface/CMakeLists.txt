add_library(DebuggerInterfaces INTERFACE)

target_sources(DebuggerInterfaces INTERFACE "include/DebuggerCommon.h")

target_include_directories(DebuggerInterfaces INTERFACE "include")
