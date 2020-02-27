add_executable(okon-cli
    main.cpp
)

target_link_libraries(okon-cli
    PRIVATE
        okon
)

target_include_directories(okon-cli
    PRIVATE
        ${OKON_DIR}
        ${OKON_INCLUDE_DIR}
)

include(GNUInstallDirs)

install(
        TARGETS okon-cli
        PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_BINDIR}
)
