include(FetchContent)

cmake_minimum_required(VERSION 3.28)
project(vox LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_subdirectory(engine)
add_subdirectory(game)

enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)

if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(CREATE_LINK
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_SOURCE_DIR}/compile_commands.json"
        SYMBOLIC
    )
endif()
