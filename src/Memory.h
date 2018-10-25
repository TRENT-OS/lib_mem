#if !defined(MEMORY_H)
#define MEMORY_H

#include "Memory_Impl.h"

void*
Memory_alloc(size_t size);

void*
Memory_calloc(size_t nmemb, size_t size);

void*
Memory_realloc(void* ptr, size_t size);

void
Memory_free(void* ptr);

#endif

