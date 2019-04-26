/**
 * @addtogroup LibMem
 * @{
 *
 * @file Nvm.h
 *
 * @brief Non Volatile Memory abstract interface
 *
 * @author Carmelo Pintaudi
 *
 * Copyright (C) 2019, Hensoldt Cyber GmbH
 */
#if !defined(NVM_H)
#define NVM_H

/* Includes ------------------------------------------------------------------*/

#include "lib_compiler/compiler.h"

#include "lib_debug/Debug.h"

#include <stddef.h>
#include <stdint.h>

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

typedef struct Nvm Nvm;

typedef size_t
(*Nvm_WriteT)(Nvm* self, size_t addr, char const* buffer, size_t length);

typedef size_t
(*Nvm_ReadT)(Nvm* self, size_t addr, char* buffer, size_t length);

typedef size_t
(*Nvm_GetSizeT)(Nvm* self);

typedef void
(*Nvm_DtorT)(Nvm* self);

typedef struct
{
    Nvm_WriteT       write;
    Nvm_ReadT        read;
    Nvm_GetSizeT     getSize;
    Nvm_DtorT        dtor;
}
Nvm_Vtable;

struct Nvm
{
    const Nvm_Vtable* vtable;
};


/* Exported constants --------------------------------------------------------*/
/* Exported dynamic functions ----------------------------------------------- */

/**
 * @brief writes from 'buffer' at the most 'length' bytes into the memory at the
 * given address.
 *
 * @param self pointer to the NVM instance
 * @param addr address from where to start writing
 * @param buffer input buffer where the data to be written are
 * @param length maximum amount of bytes that can be taken from the buffer
 *
 * @return number of bytes written in to the memory, if less than 'length' then
 *  either the memory capacity is not suffient or an error occurred
 *
 */
INLINE size_t
Nvm_write(Nvm* self, size_t addr, char const* buffer, size_t length)
{
    Debug_ASSERT_SELF(self);
    return self->vtable->write(self, addr, buffer, length);
}
/**
 * @brief reads to 'buffer' at the most 'length' bytes from the memory at the
 *  givem address.
 *
 * @param self pointer to the NVM instance
 * @param addr address from where to start writing
 * @param buffer output buffer where the read bytes will be stored
 * @param length maximum amount of bytes that can be put into the buffer
 *
 * @return number of bytes read from the memory, if less than 'length' then
 *  either the memory capacity is not suffient or an error occurred
 *
 */
INLINE size_t
Nvm_read(Nvm* self, size_t addr, char* buffer, size_t length)
{
    Debug_ASSERT_SELF(self);
    return self->vtable->read(self, addr, buffer, length);
}
/**
 * @brief gets the size (capacity) of the memory
 *
 * @param self pointer to self
 * @return the size (capacity) of the memory
 *
 */
INLINE int
Nvm_getSize(Nvm* self)
{
    Debug_ASSERT_SELF(self);
    return self->vtable->getSize(self);
}

#endif /* NVM_H */
///@}
