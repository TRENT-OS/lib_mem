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

/*----------------------------------------------------------------------------*/
static void
Test_BitmapAllocator_allocateFullMemory(BitmapAllocator* bmAllocator,
                                        void** baseAddr)
{
    // Keep track of the base with the first call
    *baseAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(bmAllocator),
                                      kElementSize);
    ASSERT_NE(baseAddr, nullptr);

    for (unsigned i = 0; i < (kNumMemoryElements - 1); i++)
    {
        void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(bmAllocator),
                                           kElementSize);
        ASSERT_NE(addr, nullptr);
    }

    return;
}

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

class Test_BitmapAllocator_fullMemorySetUp : public Test_BitmapAllocator
{
    protected:
        void SetUp()
        {
            ASSERT_TRUE(BitmapAllocator_ctor(
                            &bmAllocator,
                            kElementSize,
                            kNumMemoryElements));

            Test_BitmapAllocator_allocateFullMemory(
                &bmAllocator,
                &baseAddr);
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

/*----------------------------------------------------------------------------*/
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

// Verify that once all available memory has been allocated, an additional
// request to allocate more space will fail.
TEST_F(Test_BitmapAllocator_fullMemorySetUp,
       allocate_element_after_memory_is_full_neg)
{
    baseAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                     kElementSize);
    ASSERT_EQ(baseAddr, nullptr);
}

TEST_F(Test_BitmapAllocator_fullMemorySetUp, free_allocated_element_pos)
{
    // BitmapAllocator_free() does not return anything so to test it, first
    // allocate the whole available space, free some space and reallocate to
    // test the functionality
    BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator), baseAddr);

    void* addr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(&bmAllocator),
                                       kElementSize);
    ASSERT_NE(addr, nullptr);
}

// test free all memory and then make big alloc
TEST_F(Test_BitmapAllocator_fullMemorySetUp, free_all_and_realloc_in_once)
{
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

// Free an element in the middle of the fully allocated memory and verify that
// this freed space can be reallocated successfully.
TEST_F(Test_BitmapAllocator_fullMemorySetUp,
       free_space_in_the_middle_and_reallocate_pos)
{
    // Free space in the middle of the allocated memory
    void* addr = &((uint64_t*) baseAddr)[kNumMemoryElements / 2];
    BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator), addr);

    // Verify that the freed space in the middle can be reallocated
    void* reallocatedAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(
                                                      &bmAllocator), kElementSize);
    ASSERT_EQ(reallocatedAddr, addr);
}

// Free an element in the middle of the fully allocated memory and verify that
// reallocation will fail if more space is requested than the freed gap provides.
TEST_F(Test_BitmapAllocator_fullMemorySetUp,
       free_space_in_the_middle_and_reallocate_neg)
{
    // Free space in the middle of the allocated memory
    void* addr = &((uint64_t*) baseAddr)[kNumMemoryElements / 2];
    BitmapAllocator_free(BitmapAllocator_TO_ALLOCATOR(&bmAllocator), addr);

    // Ask for more than the freed gap provides
    void* reallocatedAddr = BitmapAllocator_alloc(BitmapAllocator_TO_ALLOCATOR(
                                                      &bmAllocator),
                                                  kElementSize * 2);
    ASSERT_EQ(reallocatedAddr, nullptr);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
