file(GLOB_RECURSE SERVER_LIB_SOURCES CONFIGURE_DEPENDS "include/server/*.hpp" "src/server/*.cpp")
add_library(vox_server_lib STATIC ${SERVER_LIB_SOURCES})

target_include_directories(vox_server_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/lib)
target_include_directories(vox_server_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(vox_server_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/pch.hpp)

target_link_libraries(vox_server_lib PUBLIC vox_common)