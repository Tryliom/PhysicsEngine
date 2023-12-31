#include "Allocator.h"

#include <gtest/gtest.h>

struct TestAllocator : public ::testing::TestWithParam<std::size_t> {};
struct TestAllocatorWithAlignment : public ::testing::TestWithParam<std::pair<std::size_t, std::size_t>> {};

INSTANTIATE_TEST_SUITE_P(Allocator, TestAllocator, testing::Values(2, 5, 15, 100, 1000));
INSTANTIATE_TEST_SUITE_P(Allocator, TestAllocatorWithAlignment, testing::Values(
	std::pair<std::size_t, std::size_t>(2, 2),
	std::pair<std::size_t, std::size_t>(6, 2),
	std::pair<std::size_t, std::size_t>(16, 4),
	std::pair<std::size_t, std::size_t>(160, 8),
	std::pair<std::size_t, std::size_t>(3200, 16)
));

TEST_P(TestAllocator, LinearConstructor)
{
	std::size_t size = GetParam();
	void* ptr = std::malloc(size);

	LinearAllocator allocator = LinearAllocator(ptr, size);

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

	LinearAllocator allocator = LinearAllocator(ptr, size);

	void* allocatedPtr = allocator.Allocate(size, alignment);

	EXPECT_EQ(allocator.GetAllocations(), 1);
	EXPECT_EQ(allocatedPtr, ptr);
}

TEST_P(TestAllocatorWithAlignment, LinearAllocateMultiple)
{
	std::size_t size = GetParam().first;
	std::size_t alignment = GetParam().second;
	void* ptr = std::malloc(size * 2);

	LinearAllocator allocator = LinearAllocator(ptr, size * 2);

	EXPECT_EQ(allocator.GetAllocations(), 0);
	EXPECT_EQ(allocator.GetSize(), size * 2);

	void* allocatedPtr = allocator.Allocate(size, alignment);
	void* allocatedPtr2 = allocator.Allocate(size, alignment);

	EXPECT_EQ(allocator.GetAllocations(), 2);
	EXPECT_EQ(allocatedPtr, ptr);
	EXPECT_EQ(allocatedPtr2, reinterpret_cast<void*>(reinterpret_cast<std::size_t>(ptr) + size));
}

// ProxyAllocator tests

TEST_P(TestAllocator, ProxyConstructor)
{
	std::size_t size = GetParam();
	void* ptr = std::malloc(size);

	LinearAllocator allocator = LinearAllocator(ptr, size);
	ProxyAllocator proxy = ProxyAllocator(allocator);

	EXPECT_EQ(proxy.GetRootPtr(), ptr);
	EXPECT_EQ(proxy.GetCurrentPtr(), ptr);
	EXPECT_EQ(proxy.GetSize(), size);
	EXPECT_EQ(proxy.GetAllocations(), 0);
}

TEST_P(TestAllocatorWithAlignment, ProxyAllocate)
{
	std::size_t size = GetParam().first;
	std::size_t alignment = GetParam().second;
	void* ptr = std::malloc(size);

	LinearAllocator allocator = LinearAllocator(ptr, size);
	ProxyAllocator proxy = ProxyAllocator(allocator);

	void* allocatedPtr = proxy.Allocate(size, alignment);

	EXPECT_EQ(proxy.GetAllocations(), 1);
	EXPECT_EQ(allocatedPtr, ptr);
}

TEST_P(TestAllocatorWithAlignment, ProxyAllocateMultiple)
{
	std::size_t size = GetParam().first;
	std::size_t alignment = GetParam().second;
	void* ptr = std::malloc(size * 2);

	LinearAllocator allocator = LinearAllocator(ptr, size * 2);
	ProxyAllocator proxy = ProxyAllocator(allocator);

	EXPECT_EQ(proxy.GetAllocations(), 0);
	EXPECT_EQ(proxy.GetSize(), size * 2);

	void* allocatedPtr = proxy.Allocate(size, alignment);
	void* allocatedPtr2 = proxy.Allocate(size, alignment);

	EXPECT_EQ(proxy.GetAllocations(), 2);
	EXPECT_EQ(allocatedPtr, ptr);
	EXPECT_EQ(allocatedPtr2, reinterpret_cast<void*>(reinterpret_cast<std::size_t>(ptr) + size));
}

// HeapAllocator tests

TEST_P(TestAllocatorWithAlignment, HeapAllocate)
{
	std::size_t size = GetParam().first;
	std::size_t alignment = GetParam().second;

	HeapAllocator allocator;

	void* allocatedPtr = allocator.Allocate(size, alignment);

	EXPECT_TRUE(allocatedPtr != nullptr);
}

TEST_P(TestAllocatorWithAlignment, HeapAllocateMultiple)
{
	std::size_t size = GetParam().first;
	std::size_t alignment = GetParam().second;

	HeapAllocator allocator;

	void* allocatedPtr = allocator.Allocate(size, alignment);
	void* allocatedPtr2 = allocator.Allocate(size, alignment);

	EXPECT_TRUE(allocatedPtr != nullptr);
	EXPECT_TRUE(allocatedPtr2 != nullptr);
	EXPECT_TRUE(allocatedPtr != allocatedPtr2);
}

// FreeListAllocator tests

TEST_P(TestAllocator, FreeListConstructor)
{
	std::size_t size = GetParam();
	void* ptr = std::malloc(size);

	FreeListAllocator allocator = FreeListAllocator(ptr, size);

	EXPECT_EQ(allocator.GetRootPtr(), ptr);
	EXPECT_EQ(allocator.GetSize(), size);
	EXPECT_EQ(allocator.GetAllocations(), 0);
}

TEST_P(TestAllocatorWithAlignment, FreeListAllocate)
{
	std::size_t size = GetParam().first;
	std::size_t alignment = GetParam().second;
	void* ptr = std::malloc(size + 16);

	FreeListAllocator allocator = FreeListAllocator(ptr, size + 16);

	void* allocatedPtr = allocator.Allocate(size, alignment);

	EXPECT_EQ(allocator.GetAllocations(), 1);
	EXPECT_TRUE(allocatedPtr != nullptr);
}

TEST_P(TestAllocatorWithAlignment, FreeListAllocateMultiple)
{
	std::size_t size = GetParam().first;
	std::size_t alignment = GetParam().second;
	void* ptr = std::malloc((size + sizeof(AllocationHeader)) * 2);

	FreeListAllocator allocator = FreeListAllocator(ptr, (size + sizeof(AllocationHeader)) * 2);

	EXPECT_EQ(allocator.GetAllocations(), 0);
	EXPECT_EQ(allocator.GetSize(), (size + sizeof(AllocationHeader)) * 2);

	void* allocatedPtr = allocator.Allocate(size, alignment);
	void* allocatedPtr2 = allocator.Allocate(size, alignment);

	EXPECT_EQ(allocator.GetAllocations(), 2);
	EXPECT_TRUE(allocatedPtr != nullptr);
	EXPECT_TRUE(allocatedPtr2 != nullptr);
	EXPECT_TRUE(allocatedPtr != allocatedPtr2);
}

TEST_P(TestAllocatorWithAlignment, FreeListAllocatorDeallocate)
{
	std::size_t size = GetParam().first;
	std::size_t alignment = GetParam().second;
	void* ptr = std::malloc((size + sizeof(AllocationHeader)) * 2);

	FreeListAllocator allocator = FreeListAllocator(ptr, (size + sizeof(AllocationHeader)) * 2);

	void* allocatedPtr = allocator.Allocate(size, alignment);
	void* allocatedPtr2 = allocator.Allocate(size, alignment);

	EXPECT_EQ(allocator.GetAllocations(), 2);
	EXPECT_TRUE(allocatedPtr != nullptr);
	EXPECT_TRUE(allocatedPtr2 != nullptr);
	EXPECT_TRUE(allocatedPtr != allocatedPtr2);

	allocator.Deallocate(allocatedPtr);
	allocator.Deallocate(allocatedPtr2);

	EXPECT_EQ(allocator.GetAllocations(), 0);
}