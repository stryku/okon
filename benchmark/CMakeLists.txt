add_executable(pwned_sonic_benchmarks
    exists_benchmark.cpp
)

target_include_directories(pwned_sonic_benchmarks
    PRIVATE
        ${benchmark_INCLUDE_DIRS}
)

target_link_libraries(pwned_sonic_benchmarks
    PRIVATE
        ${benchmark_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)
