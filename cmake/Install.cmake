include(GNUInstallDirs)
# Calc library

# cmake-format: off
install(
    TARGETS RetroDebugger RetroDebuggerCommon
    EXPORT RetroDebuggerLib
    ARCHIVE COMPONENT runtime
    LIBRARY COMPONENT runtime
    RUNTIME COMPONENT runtime
    FILE_SET HEADERS COMPONENT runtime
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT runtime)
# cmake-format: on

if(UNIX)
    install(CODE "execute_process(COMMAND ldconfig)" COMPONENT runtime)
endif()

install(
    EXPORT RetroDebuggerLib
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/RetroDeb/cmake
    NAMESPACE RetroDeb::
    COMPONENT runtime)

install(
    FILES "RetroDebuggerConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/RetroDeb/cmake
    COMPONENT runtime)

# CPack configuration
#set(CPACK_PACKAGE_VENDOR "YodasYodeler")
#set(CPACK_PACKAGE_CONTACT "email@example.com")
#set(CPACK_PACKAGE_DESCRIPTION "")
#include(CPack)
