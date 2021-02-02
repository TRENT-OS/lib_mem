#
# The IO Library
#
# Copyright (C) 2020, HENSOLDT Cyber GmbH
#

cmake_minimum_required(VERSION 3.17)

#-------------------------------------------------------------------------------
project(lib_mem C)

#-------------------------------------------------------------------------------
# LIBRARY
#-------------------------------------------------------------------------------
add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME}
    INTERFACE
        "src/AllocatorSafe.c"
        "src/BitmapAllocator.c"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE
        "include"
)

if (DEBUG_CONFIG_H_FILE)
    target_compile_definitions(${PROJECT_NAME}
        INTERFACE
            "DEBUG_CONFIG_H_FILE=${DEBUG_CONFIG_H_FILE}"
    )
endif ()

if (MEMORY_CONFIG_H_FILE)
    target_compile_definitions(${PROJECT_NAME}
        INTERFACE
            "MEMORY_CONFIG_H_FILE=${MEMORY_CONFIG_H_FILE}"
        )
endif ()

target_link_libraries(${PROJECT_NAME}
    INTERFACE
        os_core_api
        lib_compiler
        lib_debug
        lib_logs
        lib_mem
        lib_utils
        lib_osal
)

#-------------------------------------------------------------------------------
# TESTS
#-------------------------------------------------------------------------------
if (BUILD_TESTING)
    include("${TEST_MAIN_DIR}/test.cmake")
    add_test_target(${PROJECT_NAME}
        SOURCES
            "test/src/Test_BitmapAllocator.cpp"
        MOCKS
            lib_compiler_mocks
            lib_debug_mocks
            lib_utils_mocks
            lib_logs_mocks
            lib_osal_mocks
    )
endif ()

#-------------------------------------------------------------------------------
# MOCKS
#-------------------------------------------------------------------------------
if (BUILD_TESTING)
    add_library("${PROJECT_NAME}_mocks" INTERFACE)

    target_include_directories("${PROJECT_NAME}_mocks"
        INTERFACE
            "mocks/include"
            "include"
    )

    target_compile_definitions("${PROJECT_NAME}_mocks"
        INTERFACE
            -DMEMORY_CONFIG_H_FILE=mem_config.h
    )

    target_link_libraries("${PROJECT_NAME}_mocks"
        INTERFACE
            lib_compiler_mocks
    )
endif ()
