add_executable(okon_benchmarks
    exists_benchmark.cpp
)

if(NOT OKON_BENCHMARK_FILE)
    message(FATAL_ERROR "Set OKON_BENCHMARK_PREPARED_DIR variable and try again")
endif()

if(NOT OKON_BENCHMARK_PREPARED_DIR)
    message(FATAL_ERROR "Set OKON_BENCHMARK_PREPARED_DIR variable and try again")
endif()

target_include_directories(okon_benchmarks
    PRIVATE
        ${OKON_DIR}
        ${benchmark_INCLUDE_DIRS}
)

target_link_libraries(okon_benchmarks
    PRIVATE
        okon
        ${benchmark_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)

target_compile_definitions(okon_benchmarks
    PRIVATE
        -DOKON_BENCHMARK_FILE="${OKON_BENCHMARK_FILE}"
        -DOKON_BENCHMARK_PREPARED_DIR="${OKON_BENCHMARK_PREPARED_DIR}"
)
