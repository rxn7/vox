cmake_minimum_required(VERSION 4.0)
project(vox LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

include(FetchContent)

add_compile_definitions(B_PRODUCTION_MODE=ON)

include(cmake/imgui.cmake)
include(cmake/glad.cmake)
include(cmake/glfw.cmake)
include(cmake/glm.cmake)
include(cmake/embed.cmake)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(vox ${PROJECT_SOURCES})

target_compile_options(vox PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/EHs-c- /GR- /D_HAS_EXCEPTIONS=0>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fno-exceptions -fno-rtti>
)

target_compile_options(vox PRIVATE
    $<$<CONFIG:Release>:
        $<$<CXX_COMPILER_ID:MSVC>:/Ot /Oy /fp:fast /GS->
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-O3 -fomit-frame-pointer -ffast-math -fno-stack-protector>
    >
)

target_include_directories(vox PRIVATE src ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(vox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.hpp)
target_link_libraries(vox PRIVATE imgui glfw glad_gl_core_43 glm::glm)

add_subdirectory(assets)
add_dependencies(vox vox_copy_assets)

file(CREATE_LINK
    "${CMAKE_BINARY_DIR}/compile_commands.json"
    "${CMAKE_SOURCE_DIR}/compile_commands.json"
    SYMBOLIC
)
