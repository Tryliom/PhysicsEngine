#include "Allocator.h"

#include "gtest/gtest.h"

// LinearAllocator

struct TestAllocator : public ::testing::TestWithParam<std::size_t> {};
struct TestAllocatorWithAlignment : public ::testing::TestWithParam<std::pair<std::size_t, std::size_t>> {};

INSTANTIATE_TEST_SUITE_P(Allocator, TestAllocator, testing::Values(2, 5, 15, 100, 1000));
INSTANTIATE_TEST_SUITE_P(Allocator, TestAllocatorWithAlignment, testing::Values(
	std::pair<std::size_t, std::size_t>(2, 2),
	std::pair<std::size_t, std::size_t>(6, 2),
	std::pair<std::size_t, std::size_t>(16, 4),
	std::pair<std::size_t, std::size_t>(160, 8),
	std::pair<std::size_t, std::size_t>(3200, 16),
	// Value that cannot be aligned
	std::pair<std::size_t, std::size_t>(5, 2),
    std::pair<std::size_t, std::size_t>(15, 4),
    std::pair<std::size_t, std::size_t>(100, 8)
));

TEST_P(TestAllocator, LinearConstructor)
{
	std::size_t size = GetParam();
	void* ptr = std::malloc(size);

	Physics::LinearAllocator allocator = Physics::LinearAllocator(ptr, size);

	EXPECT_EQ(allocator.GetRootPtr(), ptr);
	EXPECT_EQ(allocator.GetCurrentPtr(), ptr);
	EXPECT_EQ(allocator.GetSize(), size);
	EXPECT_EQ(allocator.GetAllocations(), 0);
}

TEST_P(TestAllocatorWithAlignment, LinearAllocate)
{
	std::size_t size = GetParam().first;
	std::size_t alignment = GetParam().second;
	void* ptr = std::malloc(size);

	Physics::LinearAllocator allocator = Physics::LinearAllocator(ptr, size);

	void* allocatedPtr = allocator.Allocate(size, alignment);

	EXPECT_EQ(allocator.GetAllocations(), 1);
	EXPECT_EQ(allocatedPtr, ptr);
}

TEST_P(TestAllocatorWithAlignment, LinearAllocateMultiple)
{
	std::size_t size = GetParam().first;
	std::size_t alignment = GetParam().second;
	void* ptr = std::malloc(size * 2);

	Physics::LinearAllocator allocator = Physics::LinearAllocator(ptr, size * 2);

	EXPECT_EQ(allocator.GetAllocations(), 0);
	EXPECT_EQ(allocator.GetSize(), size * 2);

	void* allocatedPtr = allocator.Allocate(size, alignment);
	void* allocatedPtr2 = allocator.Allocate(size, alignment);

	if (size % alignment != 0)
	{
		EXPECT_EQ(allocator.GetAllocations(), 1);
		EXPECT_EQ(allocatedPtr, ptr);
		EXPECT_EQ(allocatedPtr2, nullptr);
		return;
	}

	EXPECT_EQ(allocator.GetAllocations(), 2);
	EXPECT_EQ(allocatedPtr, ptr);
	EXPECT_EQ(allocatedPtr2, reinterpret_cast<void*>(reinterpret_cast<std::size_t>(ptr) + size));
}
