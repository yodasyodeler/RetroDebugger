# See if it's already been provided to us by a parent project
if(NOT TARGET magic_enum)
    cpmaddpackage("gh:Neargye/magic_enum#e046b69") # v0.9.7
    set_target_properties(magic_enum PROPERTIES FOLDER "Dependencies")
endif()
