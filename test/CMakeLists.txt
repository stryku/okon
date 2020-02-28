function(okon_add_test name file)
    add_executable(${name}
            ${file}
            btree_tests_utils.hpp
            memory_storage.hpp
    )

    target_link_libraries(${name}
        PRIVATE
            okon
            gmock_main
    )

    target_include_directories(${name}
        PRIVATE
            ${OKON_DIR}
    )

    add_test(${name} ${name})
endfunction()

okon_add_test(sorted_insert_test btree_sorted_keys_inserter_test.cpp)
okon_add_test(btree_test btree_test.cpp)

option(OKON_WITH_HEAVY_TEST "Add heavy test target" OFF)
if(OKON_WITH_HEAVY_TEST)
    add_subdirectory(heavy_test)
endif()

if(NOT WIN32)
    add_custom_target(RUN_ALL_TEST COMMAND ${CMAKE_CTEST_COMMAND})
endif()
