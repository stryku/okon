add_executable(okon_caller okon_caller.cpp)

target_include_directories(okon_caller
    PRIVATE
        ${OKON_DIR}
        ${OKON_INCLUDE_DIR}
        ${benchmark_INCLUDE_DIRS}
)

target_link_libraries(okon_caller
    PRIVATE
        okon
        ${benchmark_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)

set(ALL_VARIABLES_PRESENT TRUE)

if(NOT OKON_BENCHMARK_ORIGINAL_DB)
    message(SEND_ERROR "OKON_BENCHMARK_ORIGINAL_DB variable not provided. \
                        It should be a path to the original hashes database file.")
    set(ALL_VARIABLES_PRESENT FALSE)
endif()

if(NOT OKON_BENCHMARK_NUMBER_OF_HASHES_TO_BENCHMARK)
    message(SEND_ERROR "OKON_BENCHMARK_NUMBER_OF_HASHES_TO_BENCHMARK variable not provided. \
                        It should indicate how many hashes from original file should be chosen randomly.")
    set(ALL_VARIABLES_PRESENT FALSE)
endif()

if(NOT OKON_BENCHMARK_NUMBER_OF_HASHES_IN_ORIGINAL_DB)
    message(SEND_ERROR "OKON_BENCHMARK_NUMBER_OF_HASHES_IN_ORIGINAL_DB variable not provided. \
                        It should indicate how many hashes are in the original file.")
    set(ALL_VARIABLES_PRESENT FALSE)
endif()

if(NOT OKON_BENCHMARK_SEED)
    message("OKON_BENCHMARK_SEED not set. Setting default value: 0.")
    set(OKON_BTREE_BENCHMARK_SEED 0)
endif()

if(NOT ALL_VARIABLES_PRESENT)
    message(FATAL_ERROR "Please provide above variables to declare B-tree benchmark")
endif()

add_custom_target(okon_btree_benchmark
    COMMAND python3
            ${CMAKE_CURRENT_SOURCE_DIR}/okon_btree_benchmark.py
            $<TARGET_FILE:okon_caller> # Path to okon_caller binary
            ${OKON_BENCHMARK_NUMBER_OF_HASHES_TO_BENCHMARK}
            ${OKON_BENCHMARK_ORIGINAL_DB}
            ${OKON_BENCHMARK_NUMBER_OF_HASHES_IN_ORIGINAL_DB}
            ${OKON_BENCHMARK_BTREE_FILE}
            ${OKON_BENCHMARK_SEED}

    COMMENT "Benchmarking B-tree. Parameters: \n \
             Number of hashes to benchmark:        ${OKON_BENCHMARK_NUMBER_OF_HASHES_TO_BENCHMARK}\n \
             Original db file:                     ${OKON_BENCHMARK_ORIGINAL_DB}\n \
             Number of hashes in original db file: ${OKON_BENCHMARK_NUMBER_OF_HASHES_IN_ORIGINAL_DB}\n \
             B-tree file:                          ${OKON_BENCHMARK_BTREE_FILE}\n \
             Seed:                                 ${OKON_BENCHMARK_SEED}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
