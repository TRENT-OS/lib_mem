/**
 * @addtogroup LibMem
 * @{
 *
 * @file AllocatorSafe.h
 *
 * @brief a thread safe version of bitmap based allocator
 *
 *
 * @author Carmelo Pintaudi
 *
 * Copyright (C) 2019, Hensoldt Cyber GmbH
 */
#if !defined(ALLOCATOR_SAFE_H)
#define ALLOCATOR_SAFE_H

/* Includes ------------------------------------------------------------------*/

#include "lib_mem/Allocator.h"
#include "lib_osal/Mutex.h"

/* Exported macro ------------------------------------------------------------*/

#define AllocatorSafe_TO_ALLOCATOR(self)  (&(self)->parent)

/* Exported types ------------------------------------------------------------*/

typedef struct AllocatorSafe AllocatorSafe;

struct AllocatorSafe
{
    Allocator   parent;
    Allocator*  impl;
    Mutex*      mutex;
};


/* Exported constants --------------------------------------------------------*/
/* Exported dynamic functions ----------------------------------------------- */
/* Exported static functions -------------------------------------------------*/

bool
AllocatorSafe_ctor(AllocatorSafe*   self,
                   Allocator*       impl,
                   Mutex*           mutex);
void*
AllocatorSafe_alloc(Allocator* allocator, size_t size);

void
AllocatorSafe_free(Allocator* allocator, void* ptr);

void
AllocatorSafe_dtor(Allocator* allocator);

#endif /* ALLOCATOR_SAFE_H */
///@}
