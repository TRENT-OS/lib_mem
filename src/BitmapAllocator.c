/* Includes ------------------------------------------------------------------*/

#include "lib_mem/BitmapAllocator.h"
#include "lib_debug/Debug.h"

#include <stdbool.h>
#include <limits.h>


/* Defines -------------------------------------------------------------------*/
#define BITS_IN_A_BITMAP_SLOT(self) (sizeof((self)->bitmap[0]) * CHAR_BIT)
#define SLOT(self, elementNum) ((elementNum) / BITS_IN_A_BITMAP_SLOT((self)))
#define OFFSET(self, elementNum) ((elementNum) % BITS_IN_A_BITMAP_SLOT((self)))
#define ELEMENT_NUM(self, slot, offset)\
    ((slot) * BITS_IN_A_BITMAP_SLOT((self)) + (offset))
// precondition is that ptr is within our boundaries
#define TO_ELEMENT_NUM(self, ptr)\
    (((ptr) - (self)->baseAddr) / (self)->elementSize)
#define TO_MEM_ADDR(self, elNum)\
    (((self)->baseAddr) + elNum * (self)->elementSize)

/* Private functions prototypes ----------------------------------------------*/

INLINE bool
isInRange(BitmapAllocator* self, void* ptr)
{
    return (ptr != NULL
            && ptr >= self->baseAddr
            && ptr < self->baseAddr + self->numElements * self->elementSize);
}

INLINE bool
isAllocatedElem(BitmapAllocator* self, size_t elementNum)
{
    Debug_ASSERT(elementNum < self->numElements);

    size_t slot     = SLOT(self, elementNum);
    size_t offset   = OFFSET(self, elementNum);

    return BitMap_GET_BIT(self->bitmap[slot], offset);
}

INLINE bool
isAllocated(BitmapAllocator* self, void* ptr)
{
    bool retval = true;

    if (!isInRange(self, ptr))
    {
        retval = false;
    }
    else
    {
        retval = isAllocatedElem(self, TO_ELEMENT_NUM(self, ptr));
    }
    return retval;
}

INLINE bool
isBoundaryElem(BitmapAllocator* self, size_t elementNum)
{
    Debug_ASSERT(elementNum < self->numElements);

    size_t slot     = SLOT(self, elementNum);
    size_t offset   = OFFSET(self, elementNum);

    return BitMap_GET_BIT(self->boundaryBitmap[slot], offset);
}

INLINE bool
isBoundary(BitmapAllocator* self, void* ptr)
{
    bool retval = true;

    if (!isInRange(self, ptr))
    {
        retval = false;
    }
    else
    {
        retval = isBoundaryElem(self, TO_ELEMENT_NUM(self, ptr));
    }
    return retval;
}

INLINE void*
findContiguousFreeElements(BitmapAllocator* self, size_t numElements)
{
    size_t amount = 0;
    size_t needle = -1;

    for (size_t slot = 0; slot < BitmapAllocator_MAX_NUM_BITMAPS; slot++)
    {
        for (size_t offset = 0;
             offset < BITS_IN_A_BITMAP_SLOT(self);
             offset++)
        {
            size_t elementNum = ELEMENT_NUM(self, slot, offset);

            if (elementNum >= self->numElements)
            {
                return NULL; // out of range
            }
            else if (!BitMap_GET_BIT(self->bitmap[slot], offset))
            {
                //Debug_LOG_TRACE("%s: free at slot %zd offset %zd",
                //                __func__, slot, offset);
                needle = (-1 == needle) ? elementNum : needle;
                if (++amount >= numElements)
                {
                    //Debug_LOG_TRACE("%s: found with needle %zu",
                    //                __func__, needle);
                    return TO_MEM_ADDR(self, needle);
                }
                // else do nothing
            }
            else
            {
                //Debug_LOG_TRACE("%s: busy at slot %zd offset %zd",
                //                __func__, slot, offset);
                amount = 0;
                needle = -1;
            }
        }
    }
    return NULL;
}
// precondition is that ptr is within our boundaries and not already marked
// as allocated
INLINE void
markBitmapBusy(BitmapAllocator* self, void* ptr, size_t numElements)
{
    Debug_ASSERT(numElements);
    Debug_ASSERT(isInRange(self, ptr));
    size_t baseElementNum = TO_ELEMENT_NUM(self, ptr);
    Debug_ASSERT(baseElementNum + numElements <= self->numElements);

    for (size_t i = 0; i < numElements; i++)
    {
        size_t elementNum   = baseElementNum + i;
        size_t slot         = SLOT(self, elementNum);
        size_t offset       = OFFSET(self, elementNum);

        //Debug_LOG_TRACE("%s: setting bit %d in slot %d",
        //                __func__, offset, slot);

        Debug_ASSERT(!isAllocatedElem(self, elementNum));
        Debug_ASSERT(!isBoundaryElem(self, elementNum));
        BitMap_SET_BIT(self->bitmap[slot], offset);
    }
    // set the boundary bit
    size_t lastElementNum = baseElementNum + numElements - 1;
    BitMap_SET_BIT(self->boundaryBitmap[SLOT(self, lastElementNum)],
                   OFFSET(self, lastElementNum));
}
// precondition is that ptr is within our boundaries and already marked
// as allocated
INLINE void
markBitmapFree(BitmapAllocator* self, void* ptr, size_t numElements)
{
    Debug_ASSERT(numElements);
    Debug_ASSERT(isInRange(self, ptr));
    size_t baseElementNum = TO_ELEMENT_NUM(self, ptr);
    Debug_ASSERT(baseElementNum + numElements <= self->numElements);

    for (size_t i = 0; i < numElements; i++)
    {
        size_t elementNum   = baseElementNum + i;
        size_t slot         = SLOT(self, elementNum);
        size_t offset       = OFFSET(self, elementNum);

        //Debug_LOG_TRACE("%s: clearing bit %d in slot %d",
        //                __func__, offset, slot);

        Debug_ASSERT(isAllocatedElem(self, elementNum));
        BitMap_CLR_BIT(self->bitmap[slot], offset);
    }
    // reset the boundary bit
    size_t lastElementNum = baseElementNum + numElements - 1;
    Debug_ASSERT(isBoundaryElem(self, lastElementNum));
    BitMap_CLR_BIT(self->boundaryBitmap[SLOT(self, lastElementNum)],
                   OFFSET(self, lastElementNum));
}

// precondition is that ptr is within our boundaries
INLINE void*
findBoundaryOfAllocatedMemory(BitmapAllocator* self, void* ptr)
{
    Debug_ASSERT(isInRange(self, ptr));
    size_t elementNum = TO_ELEMENT_NUM(self, ptr);
    Debug_ASSERT(elementNum < self->numElements);

    for (size_t i = elementNum; i < self->numElements; i++)
    {
        //Debug_LOG_TRACE("%s: checking element #%d for boundary flag..",
        //                __func__, i);
        if (isBoundaryElem(self, i))
        {
            //Debug_LOG_TRACE("%s: boundary flag found!", __func__);
            return TO_MEM_ADDR(self, i);
        }
    }
    return NULL;
}

// precondition is that ptr and boundary are within our boundaries
INLINE size_t
getElementsGap(BitmapAllocator* self, void* ptr, void* boundary)
{
    Debug_ASSERT(isInRange(self, ptr));
    Debug_ASSERT(isInRange(self, boundary));

    return (boundary - ptr) / self->elementSize + 1;
}


/* Private variables ---------------------------------------------------------*/

static const Allocator_Vtable BitmapAllocator_vtable =
{
    .alloc      = BitmapAllocator_alloc,
    .free       = BitmapAllocator_free,
    .dtor       = BitmapAllocator_dtor
};


/* Public functions ----------------------------------------------------------*/

bool
BitmapAllocator_ctor(BitmapAllocator* self,
                     void* buffer,
                     size_t elementSize,
                     size_t numElements)
{
    Debug_ASSERT_SELF(self);

    Debug_LOG_TRACE("%s: buffer @%p, elementSize %zd, numElements %zd",
                    __func__, buffer, elementSize, numElements);

    bool retval = false;

    if (!numElements || NULL == buffer)
    {
        retval = false;
    }
    else
    {
        memset(self, 0, sizeof(*self));

        if (numElements >
            BITS_IN_A_BITMAP_SLOT(self) * BitmapAllocator_MAX_NUM_BITMAPS)
        {
            // the bitmap is too little, BitmapAllocator_MAX_NUM_BITMAPS needs
            // to be defined differently
            retval = false;
        }
        else
        {
            self->baseAddr    = buffer;
            self->elementSize = elementSize;
            self->numElements = numElements;

            self->parent.vtable = &BitmapAllocator_vtable;

            retval = true;
        }
    }
    return retval;
}

void*
BitmapAllocator_alloc(Allocator* allocator, size_t size)
{
    BitmapAllocator* self = (BitmapAllocator*) allocator;
    Debug_ASSERT_SELF(self);

    void* foundAddr = NULL;

    if (!size)
    {
        // do nothing
    }
    else
    {
        size_t numNeededElements = size / self->elementSize
                                   + (size % self->elementSize ? 1 : 0);
        foundAddr = findContiguousFreeElements(self, numNeededElements);

        if (NULL == foundAddr)
        {
            // do nothing;
        }
        else
        {
            markBitmapBusy(self, foundAddr, numNeededElements);
        }
    }
    Debug_LOG_TRACE("%s: size %zd, result is addr @%p",
                    __func__,
                    size,
                    foundAddr);
    return foundAddr;
}

void
BitmapAllocator_free(Allocator* allocator, void* ptr)
{
    BitmapAllocator* self = (BitmapAllocator*) allocator;
    Debug_ASSERT_SELF(self);

    if (NULL == ptr || !isAllocated(self, ptr))
    {
        // do nothing
    }
    else
    {
        void* boundary      = findBoundaryOfAllocatedMemory(self, ptr);
        size_t elementsGap  = getElementsGap(self, ptr, boundary);

        markBitmapFree(self, ptr, elementsGap);
    }
    Debug_LOG_TRACE("%s: addr @%p", __func__, ptr);
}

void
BitmapAllocator_dtor(Allocator* stream)
{
    BitmapAllocator* self = (BitmapAllocator*) stream;
    Debug_ASSERT_SELF(self);
}


/* Private functions ---------------------------------------------------------*/


///@}
