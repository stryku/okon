add_library(okon STATIC
    btree.hpp
    btree_base.hpp
    btree_node.cpp
    btree_node.hpp
    btree_rebalancer.hpp
    btree_sorted_keys_inserter.hpp
    buffers_queue.cpp
    buffers_queue.hpp
    fstream_wrapper.hpp
    okon.cpp
    original_file_reader.hpp
    preparer.cpp
    preparer.hpp
    sha1_utils.hpp
    splitted_files.hpp
    splitted_files.cpp
)

target_link_libraries(okon
    PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(okon
    PRIVATE
        ${OKON_INCLUDE_DIR}
        ${OKON_3RDPARTY_DIR}
)

if(OKON_USE_SIMD)
    target_compile_definitions(okon
        PUBLIC
            OKON_USE_SIMD
    )
endif()


set_target_properties(okon
    PROPERTIES
        PUBLIC_HEADER ${OKON_INCLUDE_DIR}/okon/okon.h
)

include(GNUInstallDirs)

install(
    TARGETS okon
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/okon
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
