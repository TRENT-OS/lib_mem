#if !defined(MEMORY_IMPL_H)
#define MEMORY_IMPL_H

#define MEMORY_IMPL_XSTR(d)    MEMORY_IMPL_STR(d)
#define MEMORY_IMPL_STR(d)     #d

#if !defined(MEMORY_CONFIG_H_FILE)
#   error a configuration file must be provided! See Memory_Config.h.example
#else
#   include MEMORY_IMPL_XSTR(MEMORY_CONFIG_H_FILE)
#endif

/* we do not forsee the converse of this define because if one tries
 * to complile implicitely a system alloc.. the linker will complain about
 * the existence of the implementations */
#if defined(Memory_Config_USE_STDLIB_ALLOC)
#include <stdlib.h>

#   if defined(Memory_Config_USE_STDLIB_ALLOC_INLINE)
#       if defined(COMPILER_H_FILE)
#           include MEMORY_IMPL_XSTR(COMPILER_H_FILE)
#       else
#           include "lib_compiler/compiler.h"
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

#   else

#       define Memory_alloc(size)           malloc(size)
#       define Memory_calloc(nmemb, size)   calloc(nmemb, size)
#       define Memory_realloc(ptr, size)    realloc(ptr, size)
#       define Memory_free(ptr)             free(ptr)

#   endif

#endif

#endif //#if !defined(MEMORY_IMPL_H)
