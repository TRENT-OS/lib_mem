/*
 *  Copyright (C) 2019, Hensoldt Cyber GmbH
 */

#include <gtest/gtest.h>

extern "C"
{
#include "lib_mem/BitmapAllocator.h"
#include <stdint.h>
#include <limits.h>
}

#define NUM_MEMORY_ELEMENTS (((sizeof(BitmapAllocator_BitmapSlot) * CHAR_BIT) * 2) + 1)
// our elements are uint64_t data
#define ALLOCATOR_BUFF_SIZE (NUM_MEMORY_ELEMENTS * sizeof(uint64_t))

static BitmapAllocator bmAllocator;
static void* baseAddr = NULL;

class Test_BitmapAllocator : public testing::Test
{
    protected:
};

TEST(Test_BitmapAllocator, construction)
{
    bool ok = BitmapAllocator_ctor(&bmAllocator,
                                   sizeof(uint64_t),
                                   NUM_MEMORY_ELEMENTS);
    ASSERT_TRUE(ok);
}

static void
allocate_full_memory_until_boundary(void)
{
    void* lastAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(
                                               &bmAllocator),
                                           sizeof(uint64_t) * 2);
    ASSERT_NE(lastAddr, nullptr);

    baseAddr = lastAddr;

    // this will allocate the memory by twos elements that is why the limit of
    // the loop is set to NUM_MEMORY_ELEMENTS / 2, moreover the number of elements
    // was purposely set not to be a multiple of BitmapAllocator_BitmapSlot, two
    // extra elements are added at the enBitmapAllocator_BitmapSlot, one
    // extra element are added at the end.
    for (unsigned i = 0; i < NUM_MEMORY_ELEMENTS / 2 - 1; i++)
    {
        void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                           sizeof(uint64_t) * 2);
        ASSERT_NE(addr, nullptr);
        ASSERT_EQ((uintptr_t) addr, (uintptr_t) lastAddr + sizeof(uint64_t) * 2);
        lastAddr = addr;
    }
    void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                       sizeof(uint64_t) * 2);
    ASSERT_EQ(addr, nullptr);

    addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                 sizeof(uint64_t));
    ASSERT_NE(addr, nullptr);
}

TEST(Test_BitmapAllocator, allocate_full_memory_until_boundary)
{
    return allocate_full_memory_until_boundary();
}

TEST(Test_BitmapAllocator, create_a_hole_and_find_back_space_there)
{
    void* addr = &((uint64_t*) baseAddr)[NUM_MEMORY_ELEMENTS / 2];
    BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator), addr);
    // check that fails if require more
    void* newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(
                                              &bmAllocator),
                                          sizeof(uint64_t) * 3);
    ASSERT_EQ(newAddr, nullptr);
    // check that works if we reuire exactly the same an then free
    newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(
                                        &bmAllocator),
                                    sizeof(uint64_t) * 2);
    ASSERT_EQ((uintptr_t) newAddr, (uintptr_t) addr);
    BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator), addr);
    // check that works if we require twice an half
    newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                    sizeof(uint64_t));
    ASSERT_EQ((uintptr_t) newAddr, (uintptr_t) addr);
    newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                    sizeof(uint64_t));
    ASSERT_EQ((uintptr_t) newAddr, (uintptr_t) addr + sizeof(uint64_t));
    // check that fails again when the memory is already fully taken
    newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                    sizeof(uint64_t));
    ASSERT_EQ(newAddr, nullptr);
}

// test free all memory and then make big alloc
TEST(Test_BitmapAllocator, free_all_and_realloc)
{
    for (unsigned i = 0; i < ALLOCATOR_BUFF_SIZE; i++)
    {
        BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                             &((char*) baseAddr)[i]);
    }
    void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                       ALLOCATOR_BUFF_SIZE + 1);
    ASSERT_EQ(addr, nullptr);
    allocate_full_memory_until_boundary();
}

// test free all memory and then make big alloc
TEST(Test_BitmapAllocator, free_all_and_realloc_in_once)
{
    for (unsigned i = 0; i < ALLOCATOR_BUFF_SIZE; i++)
    {
        BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                             &((char*) baseAddr)[i]);
    }
    void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                       ALLOCATOR_BUFF_SIZE + 1);
    ASSERT_EQ(addr, nullptr);
    addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                 ALLOCATOR_BUFF_SIZE);
    ASSERT_EQ(addr, baseAddr);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
