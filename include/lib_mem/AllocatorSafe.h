/**
 * @addtogroup LibMem
 * @{
 *
 * @file AllocatorSafe.h
 *
 * @brief a thread safe version of bitmap based allocator
 *
 * Copyright (C) 2019, Hensoldt Cyber GmbH
 */
#pragma once

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

///@}
