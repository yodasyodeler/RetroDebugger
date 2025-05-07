enable_testing()
if(NOT TARGET GTest::gtest)
    cpmaddpackage("gh:google/googletest#52eb810") # v1.17.0
    set_target_properties(gtest PROPERTIES FOLDER "Dependencies/UnitTests")
    set_target_properties(gtest_main PROPERTIES FOLDER "Dependencies/UnitTests")
    set_target_properties(gmock PROPERTIES FOLDER "Dependencies/UnitTests")
    set_target_properties(gmock_main PROPERTIES FOLDER "Dependencies/UnitTests")
endif()
