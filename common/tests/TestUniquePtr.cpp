#include "UniquePtr.h"

#include <gtest/gtest.h>

struct TestUniquePtrFixture : public ::testing::TestWithParam<int> {};

INSTANTIATE_TEST_SUITE_P(UniquePtr, TestUniquePtrFixture, testing::Values(
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9
));

TEST_P(TestUniquePtrFixture, Constructor)
{
	auto param = GetParam();

	UniquePtr<int> ptr(new int(param));

	EXPECT_EQ(*ptr.Get(), param);
}

TEST_P(TestUniquePtrFixture, Destructor)
{
	auto param = GetParam();
	int* createdPtr = new int(param);

	{
		UniquePtr<int> ptr(createdPtr);

		EXPECT_EQ(ptr.Get(), createdPtr);
		EXPECT_EQ(*ptr.Get(), param);

        ptr.~UniquePtr();

        EXPECT_EQ(ptr.Get(), nullptr);
	}
}

TEST_P(TestUniquePtrFixture, MoveConstructor)
{
    auto param = GetParam();
    int* createdPtr = new int(param);

    {
        UniquePtr<int> ptr(createdPtr);
        UniquePtr<int> ptr2(std::move(ptr));

        EXPECT_EQ(ptr.Get(), nullptr);
        EXPECT_EQ(ptr2.Get(), createdPtr);
        EXPECT_EQ(*ptr2.Get(), param);

        ptr2.~UniquePtr();

        EXPECT_EQ(ptr2.Get(), nullptr);
    }
}
