add_library(RetroDebuggerCommon INTERFACE)

# cmake-format: off
target_sources(RetroDebuggerCommon 
    INTERFACE
        FILE_SET 
            HEADERS
        BASE_DIRS
            include
        FILES
            include/RetroDebuggerCallbackDefines.h
            include/RetroDebuggerCommon.h)
# cmake-format: on
