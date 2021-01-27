/*
 *  Copyright (C) 2019, HENSOLDT Cyber GmbH
 */
/* Includes ------------------------------------------------------------------*/
#include "lib_mem/AllocatorSafe.h"

/* Defines -------------------------------------------------------------------*/
/* Private functions prototypes ----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static const Allocator_Vtable AllocatorSafe_vtable =
{
    .alloc      = AllocatorSafe_alloc,
    .free       = AllocatorSafe_free,
    .dtor       = AllocatorSafe_dtor
};

/* Public functions ----------------------------------------------------------*/
bool
AllocatorSafe_ctor(AllocatorSafe*   self,
                   Allocator*       impl,
                   Mutex*           mutex)
{
    Debug_ASSERT_SELF(self);

    bool retval = true;

    if (NULL == impl || NULL == mutex)
    {
        retval = false;
    }
    else
    {
        self->impl  = impl;
        self->mutex = mutex;
        self->parent.vtable = &AllocatorSafe_vtable;
    }
    return retval;
}

void*
AllocatorSafe_alloc(Allocator* allocator, size_t size)
{
    AllocatorSafe* self = (AllocatorSafe*) allocator;
    Debug_ASSERT_SELF(self);

    void* retval = NULL;

    Mutex_acquire(self->mutex);

    retval = Allocator_alloc(self->impl, size);

    Mutex_release(self->mutex);

    return retval;
}

void
AllocatorSafe_free(Allocator* allocator, void* ptr)
{
    AllocatorSafe* self = (AllocatorSafe*) allocator;
    Debug_ASSERT_SELF(self);

    Mutex_acquire(self->mutex);

    Allocator_free(self->impl, ptr);

    Mutex_release(self->mutex);
}

void
AllocatorSafe_dtor(Allocator* stream)
{
    AllocatorSafe* self = (AllocatorSafe*) stream;
    Debug_ASSERT_SELF(self);
}


/* Private functions ---------------------------------------------------------*/


///@}
