#
# Mocks provided for the unit test framework
#
# Copyright (C) 2021, HENSOLDT Cyber GmbH
#

cmake_minimum_required(VERSION 3.17)

#-------------------------------------------------------------------------------
add_library("${PROJECT_NAME}_mocks" INTERFACE)

target_include_directories("${PROJECT_NAME}_mocks"
    INTERFACE
        "include"
        "../include"
)

target_compile_definitions("${PROJECT_NAME}_mocks"
    INTERFACE
        -DMEMORY_CONFIG_H_FILE=mem_config.h
)

target_link_libraries("${PROJECT_NAME}_mocks"
    INTERFACE
        lib_compiler_mocks
)
