set(BUILDINFO_TEMPLATE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/buildInfo")

string(TIMESTAMP BUILDINFO_TIMESTAMP)
find_program(GIT_PATH git REQUIRED)
execute_process(COMMAND ${GIT_PATH} log --pretty=format:'%h' -n 1
                OUTPUT_VARIABLE BUILDINFO_COMMIT_SHA)

configure_file("${BUILDINFO_TEMPLATE_DIR}/BuildInfo.h.in"
               "${DESTINATION}/BuildInfo.h")

#[=======================================================================[.rst:
CreateBuildInfoHeader
---------------------

Creates build info header.

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

``BUILDINFO_TEMPLATE_DIR``
  Build directory of where BuildInfo.h will be placed.

``BUILDINFO_TIMESTAMP``
  Time stamp of when the BuildInfo was created.

``BUILDINFO_COMMIT_SHA``
  GIT SHA of the tip of git.

.. cmake:command:: create_build_info_header

  .. code-block:: cmake

    create_build_info_header(<target>)

  Creates a build info header located in the build folder under `buildInfo` folder.
  `buildInfo` directory will be added to the passed in target's include directories.
  The BuildInfo.h file will as be added to its source files for IDE support.
  BuildInfo.h will include C++ variables for the current repos GIT hash, CMake project version, and time stamp of when the binaries were built.

#]=======================================================================]
function(create_build_info_header target)
  target_include_directories(${target} PRIVATE ${DESTINATION})
  target_sources(${target} PRIVATE "${DESTINATION}/BuildInfo.h")
endfunction()
