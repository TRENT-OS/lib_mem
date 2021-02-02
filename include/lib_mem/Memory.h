/*
 *  lib_mem
 *
 *  Copyright (C) 2018, HENSOLDT Cyber GmbH
 */

#pragma once

#define MEMORY_XSTR(d)    MEMORY_STR(d)
#define MEMORY_STR(d)     #d

#if !defined(MEMORY_CONFIG_H_FILE)
#   error a configuration file must be provided! See Memory_Config.h.example
#else
#   include MEMORY_XSTR(MEMORY_CONFIG_H_FILE)
#endif

#include "lib_mem/Memory_Impl.h"

#include <stddef.h>

#if !defined(Memory_Config_STATIC)
void*
Memory_alloc(size_t size);

void*
Memory_calloc(size_t nmemb, size_t size);

void*
Memory_realloc(void* ptr, size_t size);

void
Memory_free(void* ptr);
#endif
