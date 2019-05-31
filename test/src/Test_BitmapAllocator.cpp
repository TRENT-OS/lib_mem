/*
 * File:   Test_BitmapAllocator
 * Author: cardone
 *
 * Created on 18-dic-2018, 20.17.35
 */

#include <gtest/gtest.h>

extern "C"
{
#include "lib_mem/BitmapAllocator.h"
#include <stdint.h>
#include <limits.h>
}

#define NUM_MEMORY_ELEMENTS ((sizeof(BitMapInt) * CHAR_BIT) * BitmapAllocator_MAX_NUM_BITMAPS)

static BitmapAllocator bmAllocator;
static char allocatorBuff[NUM_MEMORY_ELEMENTS * sizeof(uint64_t)];
static void* baseAddr = NULL;

class Test_BitmapAllocator : public testing::Test
{
    protected:
};

TEST(Test_BitmapAllocator, construction)
{
    bool ok = BitmapAllocator_ctor(&bmAllocator,
                                   allocatorBuff,
                                   sizeof(uint64_t),
                                   NUM_MEMORY_ELEMENTS);
    ASSERT_TRUE(ok);
}

TEST(Test_BitmapAllocator, allocate_full_memory_until_boundary)
{
    void* lastAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(
                                               &bmAllocator),
                                           sizeof(uint64_t) * 2);
    ASSERT_NE(lastAddr, nullptr);

    baseAddr = lastAddr;

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
}

TEST(Test_BitmapAllocator, create_a_hole_and_find_back_space_there)
{
    void* addr = &((uint64_t*) baseAddr)[NUM_MEMORY_ELEMENTS / 2];
    BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator), addr);
    void* newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(
                                              &bmAllocator),
                                          sizeof(uint64_t));
    ASSERT_EQ(newAddr, addr);
    newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                    sizeof(uint64_t));
    ASSERT_EQ((uintptr_t) newAddr, (uintptr_t) addr + sizeof(uint64_t));
    newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                    sizeof(uint64_t));
    ASSERT_EQ(newAddr, nullptr);
}

// test free all memory and then make big alloc
TEST(Test_BitmapAllocator, free_all_and_realloc_in_once)
{
    for (unsigned i = 0; i < sizeof(allocatorBuff); i++)
    {
        BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                             &((char*) baseAddr)[i]);
    }
    void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                       sizeof(allocatorBuff) + 1);
    ASSERT_EQ(addr, nullptr);
    addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                 sizeof(allocatorBuff));
    ASSERT_EQ(addr, baseAddr);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
