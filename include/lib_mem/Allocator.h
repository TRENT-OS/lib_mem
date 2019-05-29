/**
 * @addtogroup LibMem
 * @{
 *
 * @file Allocator.h
 *
 * @brief interface of a memory allocator
 *
 *
 * @author Carmelo Pintaudi
 *
 * Copyright (C) 2019, Hensoldt Cyber GmbH
 */
#if !defined(ALLOCATOR_H)
#define ALLOCATOR_H

/* Includes ------------------------------------------------------------------*/

#include "lib_compiler/compiler.h"

#include "lib_debug/Debug.h"

#include <stddef.h>
#include <string.h>


/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

typedef struct Allocator Allocator;

typedef void*
(*Allocator_AllocT)(Allocator* self, size_t size);

typedef void
(*Allocator_FreeT)(Allocator* self, void* ptr);

typedef void
(*Allocator_DtorT)(Allocator* self);

typedef struct
{
    Allocator_AllocT       alloc;
    Allocator_FreeT        free;
    Allocator_DtorT        dtor;
}
Allocator_Vtable;

struct Allocator
{
    const Allocator_Vtable* vtable;
};


/* Exported constants --------------------------------------------------------*/
/* Exported dynamic functions ----------------------------------------------- */

INLINE void*
Allocator_alloc(Allocator* self, size_t size)
{
    Debug_ASSERT_SELF(self);
    return self->vtable->alloc(self, size);
}

INLINE void
Allocator_free(Allocator* self, void* ptr)
{
    Debug_ASSERT_SELF(self);
    return self->vtable->alloc(self, ptr);
}

INLINE void
Allocator_dtor(Allocator* self)
{
    Debug_ASSERT_SELF(self);
    return self->vtable->dtor(self);
}

/* Exported static functions -------------------------------------------------*/

INLINE void*
Allocator_calloc(Allocator* self, size_t nmemb, size_t size)
{
    Debug_ASSERT_SELF(self);

    void* retval = self->vtable->alloc(self, size * nmemb);

    if (retval != NULL)
    {
        memset(retval, 0, size * nmemb);
    }
    return retval;
}

INLINE void*
Allocator_realloc(Allocator* self, void* ptr, size_t size)
{
    Debug_ASSERT_SELF(self);

    void* dest = self->vtable->alloc(self, size);

    if (dest != NULL && ptr != NULL && size != 0)
    {
        memcpy(dest, ptr, size);
    }
    if (ptr != NULL)
    {
        self->vtable->free(self, ptr);
    }
    return dest;
}

#endif /* ALLOCATOR_H */
///@}