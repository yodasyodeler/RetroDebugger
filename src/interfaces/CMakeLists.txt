add_library(RetroDebuggerInterfaces INTERFACE)

# cmake-format: off
target_sources(RetroDebuggerInterfaces 
    INTERFACE
        FILE_SET 
            HEADERS
        BASE_DIRS
            include
        FILES
            include/DebuggerError.h
            include/IDebuggerCallbacks.h
            include/RetroDebuggerCallbackDefines.h
            include/RetroDebuggerCommon.h)
# cmake-format: on
