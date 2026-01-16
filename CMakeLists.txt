cmake_minimum_required(VERSION 4.0)
project(vox LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math")

include(FetchContent)

include(cmake/imgui.cmake)
include(cmake/glad.cmake)
include(cmake/glfw.cmake)
include(cmake/glm.cmake)
include(cmake/embed.cmake)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(vox ${PROJECT_SOURCES})

target_include_directories(vox PRIVATE src)
target_link_libraries(vox PRIVATE imgui glfw glad_gl_core_43 glm::glm)
target_precompile_headers(vox PRIVATE src/pch.hpp)

add_subdirectory(assets)
