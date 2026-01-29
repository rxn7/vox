file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.hpp" "*.cpp")
add_executable(vox ${SOURCES})

target_include_directories(vox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(vox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/pch.hpp)

target_link_libraries(vox PUBLIC vox_common imgui glfw glad_gl_core_43)

add_subdirectory(embed)

add_custom_target(vox_copy_assets ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/client/assets
	COMMENT "Copying assets into binary directory"
)

add_dependencies(vox vox_copy_assets)
