# See if it's already been provided to us by a parent project
if(NOT TARGET fmtlib::fmtlib)
    cpmaddpackage("gh:fmtlib/fmt#1239137") # 11.1.4 release
    set_target_properties(fmt PROPERTIES FOLDER "Dependencies")
endif()
