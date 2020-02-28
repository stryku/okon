if (NOT OKON_HEAVY_TEST_ORIGINAL_DB)
    message(FATAL_ERROR "OKON_HEAVY_TEST_ORIGINAL_DB variable not found. It should be a path to an original database file")
endif ()

add_custom_target(okon_heavy_test
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/heavy_test.py
                        $<TARGET_FILE:okon-cli> ${OKON_HEAVY_TEST_ORIGINAL_DB}
                        ${CMAKE_CURRENT_BINARY_DIR}/heavy_test_wd/
        COMMENT  "Heavy testing on ${OKON_HEAVY_TEST_ORIGINAL_DB}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
