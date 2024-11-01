/*
 * lib_mem
 *
 * Copyright (C) 2018-2024, HENSOLDT Cyber GmbH
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * For commercial licensing, contact: info.cyber@hensoldt.net
*/

#pragma once

#if defined(Memory_Config_USE_STDLIB_ALLOC)

#include "lib_compiler/compiler.h"

#include <stdlib.h>

#   if defined(Memory_Config_USE_STDLIB_ALLOC_INLINE)
#       if defined(COMPILER_H_FILE)
#           include MEMORY_IMPL_XSTR(COMPILER_H_FILE)
#       endif

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

#   else // not defined(Memory_Config_USE_STDLIB_ALLOC_INLINE)

#       define Memory_alloc(size)           malloc(size)
#       define Memory_calloc(nmemb, size)   calloc(nmemb, size)
#       define Memory_realloc(ptr, size)    realloc(ptr, size)
#       define Memory_free(ptr)             free(ptr)

#   endif // [not] defined(Memory_Config_USE_STDLIB_ALLOC_INLINE)

#endif // defined(Memory_Config_USE_STDLIB_ALLOC)
