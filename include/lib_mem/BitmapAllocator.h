/**
 * @addtogroup LibMem
 * @{
 *
 * @file BitmapAllocator.h
 *
 * @brief a bitmap based allocator
 *
 *
 * @author Carmelo Pintaudi
 *
 * Copyright (C) 2019, Hensoldt Cyber GmbH
 */
#if !defined(BITMAP_ALLOCATOR_H)
#define BITMAP_ALLOCATOR_H

/* Includes ------------------------------------------------------------------*/

#include "lib_compiler/compiler.h"

#include "lib_mem/Allocator.h"
#include "lib_util/Bitmap.h"

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

/* Exported macro ------------------------------------------------------------*/

#define BitmapAllocator_TO_ALLOCATOR(self)  (&(self)->parent)

#define BitmapAllocator_BITMAP_SIZE(NUM_EL)\
    ((NUM_EL) / CHAR_BIT + ((NUM_EL) % CHAR_BIT ? 1 : 0))

/* Exported types ------------------------------------------------------------*/

typedef struct BitmapAllocator BitmapAllocator;
typedef BitMapInt BitmapAllocator_BitmapSlot;

struct BitmapAllocator
{
    Allocator                   parent;
    void*                       baseAddr;
    size_t                      elementSize;
    size_t                      numElements;
    size_t                      allocatedElements;
    BitmapAllocator_BitmapSlot* bitmap;
    BitmapAllocator_BitmapSlot* boundaryBitmap;
    bool                        isStatic;
};


/* Exported constants --------------------------------------------------------*/
/* Exported dynamic functions ----------------------------------------------- */
/* Exported static functions -------------------------------------------------*/

/// TODO: document

#if !defined(Memory_Config_STATIC)
bool
BitmapAllocator_ctor(BitmapAllocator* self,
                     size_t elementSize,
                     size_t numElements);
#endif

bool
BitmapAllocator_ctorStatic(BitmapAllocator* self,
                           void* buffer,
                           void* bitmap,
                           void* boundaryBitmap,
                           size_t elementSize,
                           size_t numElements);

void*
BitmapAllocator_alloc(Allocator* allocator, size_t size);

void
BitmapAllocator_free(Allocator* allocator, void* ptr);

void
BitmapAllocator_dtor(Allocator* allocator);


#endif /* BITMAP_ALLOCATOR_H */
///@}
