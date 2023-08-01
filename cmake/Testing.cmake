enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG b796f7d # v1.13.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt
#     ON
#     CACHE BOOL "" FORCE)
# option(INSTALL_GMOCK "Install GMock" OFF)
# option(INSTALL_GTEST "Install GTest" OFF)
FetchContent_MakeAvailable(googletest)
set_target_properties(gtest PROPERTIES FOLDER "Dependencies/UnitTests")
set_target_properties(gtest_main PROPERTIES FOLDER "Dependencies/UnitTests")
set_target_properties(gmock PROPERTIES FOLDER "Dependencies/UnitTests")
set_target_properties(gmock_main PROPERTIES FOLDER "Dependencies/UnitTests")
