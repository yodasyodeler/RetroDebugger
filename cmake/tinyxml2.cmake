include(FetchContent)

FetchContent_Declare(
    tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG 1dee28e # 9.0.0 release
    ) 
FetchContent_MakeAvailable(tinyxml2)
