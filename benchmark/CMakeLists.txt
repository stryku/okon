add_executable(pwned_sonic_benchmarks
    exists_benchmark.cpp
)

if(NOT PWNED_SONIC_BENCHMARK_FILE)
    message(FATAL_ERROR "Set PWNED_SONIC_BENCHMARK_FILE variable and try again")
endif()

if(NOT PWNED_SONIC_BENCHMARK_PREPARED_DIR)
    message(FATAL_ERROR "Set PWNED_SONIC_BENCHMARK_PREPARED_DIR variable and try again")
endif()

target_include_directories(pwned_sonic_benchmarks
    PRIVATE
        ${PWNED_SONIC_DIR}
        ${benchmark_INCLUDE_DIRS}
)

target_link_libraries(pwned_sonic_benchmarks
    PRIVATE
        pwned_sonic
        ${benchmark_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)

target_compile_definitions(pwned_sonic_benchmarks
    PRIVATE
        -DPWNED_SONIC_BENCHMARK_FILE="${PWNED_SONIC_BENCHMARK_FILE}"
        -DPWNED_SONIC_BENCHMARK_PREPARED_DIR="${PWNED_SONIC_BENCHMARK_PREPARED_DIR}"
)
