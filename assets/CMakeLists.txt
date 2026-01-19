add_subdirectory(embed)

add_custom_target(vox_copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/assets/textures ${PROJECT_BINARY_DIR}/assets/textures
    COMMENT "Copying assets into binary directory"
)
