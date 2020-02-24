add_executable(sorted_insert_test
    btree_sorted_keys_inserter_test.cpp
    btree_test.cpp
    btree_tests_utils.hpp
    memory_storage.hpp
)

target_link_libraries(sorted_insert_test
    PRIVATE
        pwned_sonic
        gmock_main
)

target_include_directories(sorted_insert_test
    PRIVATE
        ${PWNED_SONIC_DIR}
)
