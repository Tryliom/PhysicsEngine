#include "UniquePtr.h"

#include "gtest/gtest.h"

using namespace Physics;

struct TestUniquePtrFixture : public ::testing::TestWithParam<int> {};

INSTANTIATE_TEST_SUITE_P(UniquePtr, TestUniquePtrFixture, testing::Values(
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9
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
	}

	// Test that trying to delete again createdPtr terminates the program
	EXPECT_EXIT(delete createdPtr, ::testing::ExitedWithCode(-1073740940), "");
}