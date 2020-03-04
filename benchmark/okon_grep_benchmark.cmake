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
    set(OKON_BENCHMARK_SEED 0)
endif()

if(NOT ALL_VARIABLES_PRESENT)
    message(FATAL_ERROR "Please provide above variables to declare grep benchmark")
endif()

add_custom_target(okon_grep_benchmark
    COMMAND python3
            ${CMAKE_CURRENT_SOURCE_DIR}/okon_grep_benchmark.py
            ${OKON_BENCHMARK_NUMBER_OF_HASHES_TO_BENCHMARK}
            ${OKON_BENCHMARK_ORIGINAL_DB}
            ${OKON_BENCHMARK_NUMBER_OF_HASHES_IN_ORIGINAL_DB}
            ${OKON_BENCHMARK_SEED}

    COMMENT "Benchmarking grep. Parameters: \n \
             Number of hashes to benchmark:        ${OKON_BENCHMARK_NUMBER_OF_HASHES_TO_BENCHMARK}\n \
             Original db file:                     ${OKON_BENCHMARK_ORIGINAL_DB}\n \
             Number of hashes in original db file: ${OKON_BENCHMARK_NUMBER_OF_HASHES_IN_ORIGINAL_DB}\n \
             Seed:                                 ${OKON_BENCHMARK_SEED}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
