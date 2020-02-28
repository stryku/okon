add_executable(sorted_insert_test
    btree_sorted_keys_inserter_test.cpp
    btree_test.cpp
    btree_tests_utils.hpp
    memory_storage.hpp
)

target_link_libraries(sorted_insert_test
    PRIVATE
        okon
        gmock_main
)

target_include_directories(sorted_insert_test
    PRIVATE
        ${OKON_DIR}
)

option(OKON_WITH_HEAVY_TEST "Add heavy test target" OFF)
if(OKON_WITH_HEAVY_TEST)
    add_subdirectory(heavy_test)
endif()
