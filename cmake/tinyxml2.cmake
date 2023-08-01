include(FetchContent)
FetchContent_Declare(
    tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG 1dee28e # 9.0.0 release
)
set(tinyxml2_BUILD_TESTING OFF) # turn ON or remove to generate the unit tests for additional examples.
FetchContent_MakeAvailable(tinyxml2)
set_target_properties(tinyxml2 PROPERTIES FOLDER "Dependencies")
