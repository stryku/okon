add_library(pwned_sonic STATIC
    btree.cpp
    btree.hpp
    btree_base.hpp
    btree_node.cpp
    btree_node.hpp
    btree_sorted_keys_inserter.hpp
    preparer.cpp
    preparer.hpp
    pwned_sonic.cpp
    pwned_sonic.hpp
    sha1_utils.hpp
    splitted_files.hpp
)

target_link_libraries(pwned_sonic
    PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
)
