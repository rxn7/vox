include(FetchContent)

cmake_minimum_required(VERSION 3.28)
project(vox LANGUAGES CXX C)

# remove enable exceptions flags
if(MSVC)
	foreach(flag_var
		CMAKE_CXX_FLAGS 
		CMAKE_CXX_FLAGS_DEBUG 
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_MINSIZEREL 
		CMAKE_CXX_FLAGS_RELWITHDEBINFO)

		if(${flag_var} MATCHES "/EHsc")
			string(REPLACE "/EHsc" "" ${flag_var} "${${flag_var}}")
		endif()
		
		string(REPLACE "/EHs" "" ${flag_var} "${${flag_var}}")
		string(REPLACE "/EHc" "" ${flag_var} "${${flag_var}}")
	endforeach()
endif()


set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 23 CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE STRING "" FORCE)
set(CMAKE_CXX_EXTENSIONS OFF CACHE STRING "" FORCE)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)
set(CMAKE_COLOR_DIAGNOSTICS ON CACHE STRING "" FORCE)

include(cmake/imgui.cmake)
include(cmake/glad.cmake)
include(cmake/glfw.cmake)
include(cmake/glm.cmake)
include(cmake/embed.cmake)
include(cmake/game_networking_sockets.cmake)

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
