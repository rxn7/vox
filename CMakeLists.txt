include(FetchContent)

cmake_minimum_required(VERSION 3.28)
project(vox LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

include(cmake/imgui.cmake)
include(cmake/glad.cmake)
include(cmake/glfw.cmake)
include(cmake/glm.cmake)
include(cmake/embed.cmake)

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)

if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(CREATE_LINK
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/compile_commands.json"
        SYMBOLIC
    )
endif()
