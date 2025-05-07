if(NOT TARGET tinyxml2)
    set(tinyxml2_BUILD_TESTING OFF) # turn ON or remove to generate the unit tests for additional examples.
    cpmaddpackage("gh:leethomason/tinyxml2#9148bdf") # 9.0.0 release
    set_target_properties(tinyxml2 PROPERTIES FOLDER "Dependencies")
endif()
