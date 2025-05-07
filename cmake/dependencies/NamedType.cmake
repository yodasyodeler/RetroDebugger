# See if it's already been provided to us by a parent project
if(NOT TARGET NamedType)
    cpmaddpackage("gh:joboccara/NamedType#76668ab") # Development head
    set_target_properties(NamedType PROPERTIES FOLDER "Dependencies")
endif()
