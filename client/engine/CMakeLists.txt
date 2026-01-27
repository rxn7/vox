file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "*.hpp" "*.cpp")
add_library(vox_engine STATIC ${ENGINE_SOURCES})

target_include_directories(vox_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(vox_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(vox_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/pch.hpp)

add_subdirectory(embed)

target_link_libraries(vox_engine PUBLIC vox_common imgui glfw glad_gl_core_43)
target_link_libraries(vox_engine PRIVATE vox_common_pch)