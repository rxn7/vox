# GLAD 2.0.6 

FetchContent_Declare(
    glad 
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git 
    GIT_TAG 73db193f853e2ee079bf3ca8a64aa2eaf6459043 
    GIT_PROGRESS TRUE 
    SOURCE_SUBDIR cmake
)
FetchContent_MakeAvailable(glad)

glad_add_library(glad_gl_core_43 STATIC REPRODUCIBLE API gl:core=4.3)
