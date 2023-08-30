enable_testing()

cpmaddpackage("gh:google/googletest#b796f7d") # v1.13.0
set_target_properties(gtest PROPERTIES FOLDER "Dependencies/UnitTests")
set_target_properties(gtest_main PROPERTIES FOLDER "Dependencies/UnitTests")
set_target_properties(gmock PROPERTIES FOLDER "Dependencies/UnitTests")
set_target_properties(gmock_main PROPERTIES FOLDER "Dependencies/UnitTests")
