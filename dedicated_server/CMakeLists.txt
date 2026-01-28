file(GLOB_RECURSE DEDICATED_SERVER_SOURCES CONFIGURE_DEPENDS "include/*.hpp" "src/*.cpp")

add_executable(vox_dedicated_server ${DEDICATED_SERVER_SOURCES})
target_include_directories(vox_dedicated_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(vox_dedicated_server PUBLIC vox_common)

target_include_directories(vox_dedicated_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(vox_dedicated_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/pch.hpp)
