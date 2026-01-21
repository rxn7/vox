file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "*.hpp" "*.cpp")
add_library(vox_engine STATIC ${ENGINE_SOURCES})

target_compile_options(vox_engine PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:
        /EHs-c- 
        /GR- 
        /D_HAS_EXCEPTIONS=0
    >
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
        -fno-exceptions 
        -fno-rtti
    >
)

target_compile_options(vox_engine PUBLIC
    $<$<CONFIG:Release>:
        $<$<CXX_COMPILER_ID:MSVC>:
            /Ot 
            /Oy 
            /GS-
        >
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
            -O3 
            -fomit-frame-pointer 
            -fno-stack-protector
        >
    >
)

target_include_directories(vox_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(vox_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(vox_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/pch.hpp)

include(cmake/imgui.cmake)
include(cmake/glad.cmake)
include(cmake/glfw.cmake)
include(cmake/glm.cmake)
include(cmake/embed.cmake)

add_subdirectory(embed)

target_link_libraries(vox_engine PUBLIC imgui glfw glad_gl_core_43 glm::glm)
