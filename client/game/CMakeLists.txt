file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "*.hpp" "*.cpp")

add_executable(vox_game ${GAME_SOURCES})
target_include_directories(vox_game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(vox_game PUBLIC vox_engine vox_server_lib vox_common vox_common_pch)
target_link_libraries(vox_game PRIVATE vox_common_pch)

add_subdirectory(embed)

add_custom_target(vox_copy_assets ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/client/game/assets
	COMMENT "Copying assets into binary directory"
)
add_dependencies(vox_game vox_copy_assets)