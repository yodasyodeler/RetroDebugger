include(GNUInstallDirs)
# Calc library

# cmake-format: off
install(
    TARGETS RetroDebugger RetroDebuggerCommon
    EXPORT RetroDebuggerLib
    ARCHIVE COMPONENT development
    LIBRARY COMPONENT runtime 
    FILE_SET HEADERS
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RetroDeb
    COMPONENT runtime)
# cmake-format: on

if(UNIX)
    install(CODE "execute_process(COMMAND ldconfig)" COMPONENT runtime)
endif()

message("CMAKE_INSTALL_LIBDIR: ${CMAKE_INSTALL_LIBDIR}")
install(
    EXPORT RetroDebuggerLib
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/RetroDeb/cmake
    NAMESPACE RetroDeb::
    COMPONENT runtime)

install(FILES "RetroDebuggerConfig.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/RetroDeb/cmake)

# # Runtime
# # cmake-format: off
# install(TARGETS RetroDebugger
#     FILE_SET HEADERS 
#     PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RetroDeb
#     RUNTIME COMPONENT runtime)
# # cmake-format: on

# CPack configuration
set(CPACK_PACKAGE_VENDOR "YodasYodeler")
set(CPACK_PACKAGE_CONTACT "email@example.com")
set(CPACK_PACKAGE_DESCRIPTION "Generic emulation debugger library")
include(CPack)
