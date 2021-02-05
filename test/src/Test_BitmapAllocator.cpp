/*
 *  Copyright (C) 2019-2021, HENSOLDT Cyber GmbH
 */

#include <gtest/gtest.h>

extern "C"
{
#include "lib_mem/BitmapAllocator.h"
#include <stdint.h>
#include <limits.h>
}

// Number of elements purposely set not to be a multiple of
// BitmapAllocator_BitmapSlot
constexpr unsigned kNumMemoryElements = (((sizeof(BitmapAllocator_BitmapSlot) *
                                           CHAR_BIT) * 2) + 1);
// Test-Elements are uint64_t data
constexpr unsigned kElementSize = sizeof(uint64_t);

constexpr unsigned kAllocatorBufSize = (kNumMemoryElements* kElementSize);


class Test_BitmapAllocator : public testing::Test
{
    protected:
        BitmapAllocator bmAllocator;
        void* baseAddr = NULL;
        void SetUp()
        {
            ASSERT_TRUE(BitmapAllocator_ctor(
                            &bmAllocator,
                            kElementSize,
                            kNumMemoryElements));
        }

        void TearDown()
        {
            BitmapAllocator_dtor(BitmapAllocator_TO_ALLOCATOR(&bmAllocator));
        }
};

class Test_BitmapAllocator_parameterizedFixture :
    public ::testing::TestWithParam<int>
{
    protected:
        BitmapAllocator bmAllocator;
        void* baseAddr = NULL;
        void SetUp()
        {
            ASSERT_TRUE(BitmapAllocator_ctor(
                            &bmAllocator,
                            kElementSize,
                            kNumMemoryElements));
        }
        void TearDown()
        {
            BitmapAllocator_dtor(BitmapAllocator_TO_ALLOCATOR(&bmAllocator));
        }
};

static void
allocate_full_memory_until_boundary(BitmapAllocator* bmAllocator,
                                    void** baseAddr)
{
    void* lastAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(
                                               bmAllocator), kElementSize * 2);
    ASSERT_NE(lastAddr, nullptr);

    *baseAddr = lastAddr;

    // this will allocate the memory by twos elements that is why the limit of
    // the loop is set to kNumMemoryElements / 2, moreover the number of elements
    // was purposely set not to be a multiple of BitmapAllocator_BitmapSlot, two
    // extra elements are added at the enBitmapAllocator_BitmapSlot, one
    // extra element are added at the end.
    for (unsigned i = 0; i < kNumMemoryElements / 2 - 1; i++)
    {
        void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(bmAllocator),
                                           kElementSize * 2);
        ASSERT_NE(addr, nullptr);
        ASSERT_EQ((uintptr_t) addr, (uintptr_t) lastAddr + kElementSize * 2);
        lastAddr = addr;
    }
    void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(bmAllocator),
                                       kElementSize * 2);
    ASSERT_EQ(addr, nullptr);

    addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(bmAllocator),
                                 kElementSize);
    ASSERT_NE(addr, nullptr);
}

// Parameterized test case verifying that valid allocation requests (with a
// correct size in regards to the offered space) will succeed and invalid
// requests with sizes outside of the possible memory range will fail.
TEST_P(Test_BitmapAllocator_parameterizedFixture,
       allocate_element)
{
    int requestedElements = GetParam();

    baseAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                     kElementSize * requestedElements);

    if (requestedElements > 0 && requestedElements <= kNumMemoryElements)
    {
        // Valid values should return a pointer to the allocated space
        ASSERT_NE(baseAddr, nullptr);
    }
    else
    {
        // Invalid values should return empty
        ASSERT_EQ(baseAddr, nullptr);
    }
}

INSTANTIATE_TEST_CASE_P(allocate_elements_varying_sizes,
                        Test_BitmapAllocator_parameterizedFixture,
                        ::testing::Values(-1, 0, 1,
                                          (kNumMemoryElements / 2),
                                          kNumMemoryElements,
                                          (kNumMemoryElements + 1)));


TEST_F(Test_BitmapAllocator, free_allocated_element_pos)
{
    // BitmapAllocator_free() does not return anything so to test it, first
    // allocate the whole available space, free some space and reallocate to
    // test the functionality
    allocate_full_memory_until_boundary(&bmAllocator, &baseAddr);

    BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator), baseAddr);

    void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                       kElementSize);
    ASSERT_NE(addr, nullptr);
}

TEST_F(Test_BitmapAllocator, allocate_full_memory_until_boundary)
{
    return allocate_full_memory_until_boundary(&bmAllocator, &baseAddr);
}

TEST_F(Test_BitmapAllocator, create_a_hole_and_find_back_space_there)
{
    allocate_full_memory_until_boundary(&bmAllocator, &baseAddr);

    void* addr = &((uint64_t*) baseAddr)[kNumMemoryElements / 2];
    BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator), addr);
    // check that fails if require more
    void* newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(
                                              &bmAllocator), kElementSize * 3);
    ASSERT_EQ(newAddr, nullptr);
    // check that works if we reuire exactly the same an then free
    newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(
                                        &bmAllocator), kElementSize * 2);
    ASSERT_EQ((uintptr_t) newAddr, (uintptr_t) addr);
    BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator), addr);
    // check that works if we require twice an half
    newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                    kElementSize);
    ASSERT_EQ((uintptr_t) newAddr, (uintptr_t) addr);
    newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                    kElementSize);
    ASSERT_EQ((uintptr_t) newAddr, (uintptr_t) addr + kElementSize);
    // check that fails again when the memory is already fully taken
    newAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                    kElementSize);
    ASSERT_EQ(newAddr, nullptr);
}

// test free all memory and then make big alloc
TEST_F(Test_BitmapAllocator, free_all_and_realloc)
{
    allocate_full_memory_until_boundary(&bmAllocator, &baseAddr);

    for (unsigned i = 0; i < kAllocatorBufSize; i++)
    {
        BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                             &((char*) baseAddr)[i]);
    }
    void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                       kAllocatorBufSize + 1);
    ASSERT_EQ(addr, nullptr);
    allocate_full_memory_until_boundary(&bmAllocator, &baseAddr);
}

// test free all memory and then make big alloc
TEST_F(Test_BitmapAllocator, free_all_and_realloc_in_once)
{
    allocate_full_memory_until_boundary(&bmAllocator, &baseAddr);

    for (unsigned i = 0; i < kAllocatorBufSize; i++)
    {
        BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                             &((char*) baseAddr)[i]);
    }
    void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                       kAllocatorBufSize + 1);
    ASSERT_EQ(addr, nullptr);
    addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                 kAllocatorBufSize);
    ASSERT_EQ(addr, baseAddr);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
