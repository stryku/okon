cmake_minimum_required(VERSION 3.14)

project(pwned_sonic)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)

add_executable(pwned_sonic_client
    main.cpp
)

target_link_libraries(pwned_sonic_client
    PRIVATE
        pwned_sonic
)

target_include_directories(pwned_sonic_client
    PRIVATE
        pwned_sonic
)

option(PWNED_SONIC_WITH_BENCHMARKS "Build benchmarks (requires googlebenchmark)" OFF)
if(PWNED_SONIC_WITH_BENCHMARKS)
    find_package(Threads)
    find_package(GoogleBenchmark REQUIRED)
    add_subdirectory(benchmark)
endif()
