include(FetchContent)

FetchContent_Declare(
    NamedType
    GIT_REPOSITORY https://github.com/joboccara/NamedType.git
    GIT_TAG 76668ab)
FetchContent_MakeAvailable(NamedType)
set_target_properties(NamedType PROPERTIES FOLDER "Dependencies")
