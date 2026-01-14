cmake_minimum_required(VERSION 4.0)
project(vox LANGUAGES CXX C)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PROJECT_SOURCES
	CONFIGURE_DEPENDS
	src/*.cpp
)

add_executable(vox ${PROJECT_SOURCES})

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        73db193f853e2ee079bf3ca8a64aa2eaf6459043 #v2.0.6
    GIT_PROGRESS   TRUE
    SOURCE_SUBDIR  cmake
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl_core_43 STATIC REPRODUCIBLE LOADER API gl:core=4.3)

add_subdirectory(vendor/glfw)
target_include_directories(vox PRIVATE ${glad_SOURCE_DIR}/include)
target_link_libraries(vox PRIVATE glfw glad_gl_core_43)
target_precompile_headers(vox PRIVATE src/pch.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math")
