#include "SharedPtr.h"

#include "gtest/gtest.h"

struct TestSharedPtrFixture : public ::testing::TestWithParam<int> {};

INSTANTIATE_TEST_SUITE_P(SharedPtr, TestSharedPtrFixture, testing::Values(
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9
));

TEST_P(TestSharedPtrFixture, Constructor)
{
	auto param = GetParam();

	SharedPtr<int> ptr(new int(param));

	EXPECT_EQ(*ptr.Get(), param);
}

TEST_P(TestSharedPtrFixture, Destructor)
{
	auto param = GetParam();
	int* createdPtr = new int(param);

	{
		SharedPtr<int> ptr(createdPtr);

		EXPECT_EQ(ptr.Get(), createdPtr);
		EXPECT_EQ(*ptr.Get(), param);

        /* Cannot test it, crash instantly after the end of the scope
         * ptr.~SharedPtr();

        EXPECT_EQ(ptr.Get(), nullptr);*/
	}
}

TEST_P(TestSharedPtrFixture, CopyConstructor)
{
	auto param = GetParam();
	int* createdPtr = new int(param);

	{
		SharedPtr<int> ptr(createdPtr);
		SharedPtr<int> ptr2(ptr);

		EXPECT_EQ(ptr.Get(), createdPtr);
		EXPECT_EQ(ptr2.Get(), createdPtr);
		EXPECT_EQ(*ptr.Get(), param);
		EXPECT_EQ(*ptr2.Get(), param);
	}
}

TEST_P(TestSharedPtrFixture, CopyAssignment)
{
	auto param = GetParam();
	int* createdPtr = new int(param);

	{
		SharedPtr<int> ptr(createdPtr);
		SharedPtr<int> ptr2 = ptr;

		EXPECT_EQ(ptr.Get(), createdPtr);
		EXPECT_EQ(ptr2.Get(), createdPtr);
		EXPECT_EQ(*ptr.Get(), param);
		EXPECT_EQ(*ptr2.Get(), param);
	}
}