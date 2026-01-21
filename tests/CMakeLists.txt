add_custom_target(vox_all_tests)

function(add_vox_test test_name source_file)
    add_executable(${test_name} ${source_file})

    target_link_libraries(${test_name} PRIVATE vox_engine)

    target_include_directories(${test_name} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_test(NAME ${test_name} COMMAND ${test_name})

    add_dependencies(vox_all_tests ${test_name})
endfunction()

add_vox_test(vox_test_allocators ${CMAKE_CURRENT_SOURCE_DIR}/test_allocators.cpp)
