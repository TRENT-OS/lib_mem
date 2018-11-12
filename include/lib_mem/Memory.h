/*
 *  LibMem
 *
 *  Copyright (C) 2018, Hensoldt Cyber GmbH
 */

#if !defined(MEMORY_H)
#define MEMORY_H

#include "lib_mem/Memory_Impl.h"

void*
Memory_alloc(size_t size);

void*
Memory_calloc(size_t nmemb, size_t size);

void*
Memory_realloc(void* ptr, size_t size);

void
Memory_free(void* ptr);

#endif

