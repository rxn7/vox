FetchContent_Declare(
	glad 
	GIT_REPOSITORY https://github.com/Dav1dde/glad.git 
	GIT_TAG v2.0.8
	GIT_PROGRESS TRUE 
	SOURCE_SUBDIR cmake
)
FetchContent_MakeAvailable(glad)

glad_add_library(glad_gl_core_43 STATIC REPRODUCIBLE API gl:core=4.3)
