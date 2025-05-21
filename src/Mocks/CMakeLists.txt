
add_library(RdbMocks INTERFACE)

target_include_directories(RdbMocks INTERFACE "include")
target_sources(RdbMocks INTERFACE 
    include/MockDebuggerCallbacks.h
)