/*
 *  LibMem
 *
 *  Copyright (C) 2018, Hensoldt Cyber GmbH
 */

#pragma once

#include "lib_compiler/compiler.h"
#include "Memory_Config.h"

#include <string.h>

/* we do not forsee the converse of this define because if one tries
 * to complile implicitely a system alloc.. the linker will complain about
 * the existence of the implementations */
#if defined(Memory_Config_USE_STDLIB_ALLOC)
#include <stdlib.h>

#   if defined(Memory_Config_USE_STDLIB_ALLOC_INLINE)

INLINE void*
Memory_alloc(size_t size)
{
    return malloc(size);
}

INLINE void*
Memory_calloc(size_t nmemb, size_t size)
{
    return calloc(nmemb, size);
}

INLINE void*
Memory_realloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

INLINE void
Memory_free(void* ptr)
{
    free(ptr);
}

#   else

#       define Memory_alloc(size)           malloc(size)
#       define Memory_calloc(nmemb, size)   calloc(nmemb, size)
#       define Memory_realloc(ptr, size)    realloc(ptr, size)
#       define Memory_free(ptr)             free(ptr)

#   endif

#endif
