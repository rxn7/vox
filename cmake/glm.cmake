set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)

# GLM 1.0.3
FetchContent_Declare(
    glm 
    GIT_REPOSITORY https://github.com/g-truc/glm.git 
    GIT_TAG 8d1fd52e5ab5590e2c81768ace50c72bae28f2ed 
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(glm)
