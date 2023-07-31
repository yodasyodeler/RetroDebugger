#find_package(fmt CONFIG)
#find_package(tinyxml2 CONFIG)
#find_package(namedtype CONFIG)

add_library(RetroDebugger SHARED)

target_link_libraries(RetroDebugger PRIVATE fmt::fmt tinyxml2::tinyxml2 NamedType)

target_link_libraries(RetroDebugger PUBLIC project_warnings project_options)

include(BuildInfo)
create_build_info_header(RetroDebugger)

# Unit Tests
if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

# Sets up FILE_SET for RetroDebugger
target_sources(RetroDebugger 
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS 
            include
            ${CMAKE_BINARY_DIR}/bin/buildInfo
            ${CMAKE_CURRENT_BINARY_DIR}/include
            ${CMAKE_BINARY_DIR}/configured_files/include/
        FILES 
            include/RetroDebuggerApi.h
            include/RetroDebuggerCommon.h
            ${CMAKE_BINARY_DIR}/bin/buildInfo/BuildInfo.h
            ${CMAKE_CURRENT_BINARY_DIR}/include/RetroDebugger_export.h
            ${CMAKE_BINARY_DIR}/configured_files/include/RetroDebuggerTests_assets.h
)

# Add Source files and assets
add_subdirectory(assets) # Private assets used by unit tests
add_subdirectory(include) # Files shared with RetroDebugger and binaries using this library
add_subdirectory(source) # Files private to RetroDebugger and unit tests
