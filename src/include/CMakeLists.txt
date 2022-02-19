target_sources(RetroDebugger PUBLIC DebuggerApi.h DebuggerCommon.h)
source_group(include REGULAR_EXPRESSION *.h)
target_include_directories(RetroDebugger PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if(ENABLE_RETRODEBUGGER_UNIT_TESTS)
    target_sources(RetroDebuggerTests PRIVATE DebuggerApi.h DebuggerCommon.h)
    target_include_directories(RetroDebuggerTests PRIVATE ${CMAKE_CURRENT_LIST_DIR})
endif()
