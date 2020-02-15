cmake_minimum_required(VERSION 3.14)

project(pwned_lightning)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)

add_executable(pwned_sonic_client
    main.cpp
)

target_link_libraries(pwned_sonic_client
    PRIVATE
        pwned_sonic
)
