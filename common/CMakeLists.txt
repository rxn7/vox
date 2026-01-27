file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS "*.hpp" "*.cpp")
add_library(vox_common STATIC ${COMMON_SOURCES})

target_compile_options(vox_common PUBLIC
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

target_compile_options(vox_common PUBLIC
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

target_include_directories(vox_common PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(vox_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(vox_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/pch.hpp)

target_link_libraries(vox_common PUBLIC 
	glm::glm 
	# GameNetworkingSockets_s 
	# libprotobuf
	# absl::log_internal_check_op
)