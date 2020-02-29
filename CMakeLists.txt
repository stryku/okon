cmake_minimum_required(VERSION 3.13)

project(okon)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 17)

find_package(Threads)

set(OKON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(OKON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(lib)

option(OKON_WITH_CLI "Build okon-cli binary" OFF)
if(OKON_WITH_CLI)
    add_subdirectory(okon-cli)
endif()

option(OKON_WITH_BENCHMARKS "Build benchmarks (requires googlebenchmark)" OFF)
if(OKON_WITH_BENCHMARKS)
    find_package(GoogleBenchmark REQUIRED)
    add_subdirectory(benchmark)
endif()

option(OKON_WITH_TESTS "Build tests" OFF)
if(OKON_WITH_TESTS)
    configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
            ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
            EXCLUDE_FROM_ALL)

    if (CMAKE_VERSION VERSION_LESS 2.8.11)
        include_directories("${gtest_SOURCE_DIR}/include")
    endif()

    include(CTest)
    add_subdirectory(test)
endif()
